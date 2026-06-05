#include <awsmock/agw/ProxySession.h>

namespace Awsmock::Agw {

    ProxySession::ProxySession(net::io_context &ioc, tcp::socket &&socket, const Dto::ApiGateway::ProxyConfig &cfg) : _ioc(ioc), _clientStream(std::move(socket)), _resolver(net::make_strand(ioc)), _cfg(cfg) {}

    void ProxySession::Run() {
        net::dispatch(_clientStream.get_executor(), beast::bind_front_handler(&ProxySession::DoRead, shared_from_this()));
    }

    // ── Phase 1: read the client's HTTP request ──────────────────────────────

    void ProxySession::DoRead() {
        _reqParser.emplace();
        _clientBuffer.clear();
        _reqParser->body_limit(_cfg.bodyLimitBytes);
        _clientStream.expires_after(std::chrono::seconds(_cfg.timeoutSeconds));

        http::async_read(_clientStream, _clientBuffer, *_reqParser, beast::bind_front_handler(&ProxySession::OnRead, shared_from_this()));
    }

    void ProxySession::OnRead(const beast::error_code &ec, std::size_t) {
        if (ec == http::error::end_of_stream) return DoClose();
        if (ec) {
            log_debug << "Client read: " << ec.message();
            return;
        }
        _req = _reqParser->release();
        PrepareForwardRequest();

        // Create a fresh backend stream for this request (per-request connection).
        _backendStream.emplace(net::make_strand(_ioc));

        _resolver.async_resolve(_cfg.targetHost, std::to_string(_cfg.targetPort), beast::bind_front_handler(&ProxySession::OnResolve, shared_from_this()));
    }

    // ── Phase 2: connect to backend ──────────────────────────────────────────

    void ProxySession::OnResolve(const beast::error_code &ec, const tcp::resolver::results_type &results) {
        if (ec) {
            log_error << "Resolve '" << _cfg.targetHost << "': " << ec.message();
            return SendBadGateway("DNS resolution failed");
        }
        _backendStream->expires_after(std::chrono::seconds(_cfg.timeoutSeconds));
        _backendStream->async_connect(results, beast::bind_front_handler(&ProxySession::OnConnect, shared_from_this()));
    }

    void ProxySession::OnConnect(const beast::error_code &ec, tcp::endpoint) {
        if (ec) {
            log_error << "Connect to " << _cfg.targetHost << ":" << _cfg.targetPort << ": " << ec.message();
            return SendBadGateway("Connection to backend refused");
        }
        _backendStream->expires_after(std::chrono::seconds(_cfg.timeoutSeconds));
        http::async_write(*_backendStream, _req, beast::bind_front_handler(&ProxySession::OnForward, shared_from_this()));
    }

    // ── Phase 3: receive the backend's response ──────────────────────────────

    void ProxySession::OnForward(const beast::error_code &ec, std::size_t) {
        if (ec) {
            log_error << "Forward to backend: " << ec.message();
            return SendBadGateway("Failed to send request to backend");
        }
        _respParser.emplace();
        _respParser->body_limit(_cfg.bodyLimitBytes);
        _backendBuffer.clear();
        _backendStream->expires_after(std::chrono::seconds(_cfg.timeoutSeconds));

        http::async_read(*_backendStream, _backendBuffer, *_respParser, beast::bind_front_handler(&ProxySession::OnBackendRead, shared_from_this()));
    }

    void ProxySession::OnBackendRead(const beast::error_code &ec, std::size_t) {
        // end_of_stream is normal when the backend uses connection-close to delimit the body
        if (ec && ec != http::error::end_of_stream) {
            log_error << "Backend read: " << ec.message();
            return SendBadGateway("Backend read error");
        }
        _resp = _respParser->release();

        // Done with backend — tear it down
        beast::error_code bec;
        bec = _backendStream->socket().shutdown(tcp::socket::shutdown_both, bec);
        _backendStream.reset();

        // Propagate keep-alive preference from the original client request
        const bool keepAlive = _req.keep_alive();
        _resp.keep_alive(keepAlive);
        _resp.prepare_payload();

        log_debug << "Proxy " << _req.method_string() << " " << _req.target() << " -> " << _resp.result_int();

        _clientStream.expires_after(std::chrono::seconds(_cfg.timeoutSeconds));
        http::async_write(_clientStream, _resp, beast::bind_front_handler(&ProxySession::OnClientWrite, shared_from_this(), keepAlive));
    }

    // ── Phase 4: write response back to client ───────────────────────────────

    void ProxySession::OnClientWrite(const bool keepAlive, const beast::error_code &ec, std::size_t) {
        if (ec) {
            log_debug << "Client write: " << ec.message();
            return;
        }
        if (keepAlive) {
            DoRead();
        } else {
            DoClose();
        }
    }

    void ProxySession::DoClose() {
        beast::error_code ec;
        _clientStream.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

    // ── Helpers ──────────────────────────────────────────────────────────────

    void ProxySession::PrepareForwardRequest() {
        // Rewrite Host to match the configured backend
        _req.set(http::field::host, _cfg.targetHost + ":" + std::to_string(_cfg.targetPort));
        // Per-request backend connections — tell the backend to close after responding
        _req.set(http::field::connection, "close");
        // Strip agw-specific hop-by-hop headers
        _req.erase(http::field::proxy_connection);
        _req.erase("Proxy-Authorization");
        // Always speak HTTP/1.1 to the backend
        _req.version(11);
    }

    void ProxySession::SendBadGateway(const std::string &detail) {
        auto res = std::make_shared<http::response<http::string_body>>(http::status::bad_gateway, 11);
        res->set(http::field::content_type, "text/plain");
        res->body() = "502 Bad Gateway: " + detail;
        res->keep_alive(false);
        res->prepare_payload();

        _clientStream.expires_after(std::chrono::seconds(_cfg.timeoutSeconds));
        http::async_write(_clientStream, *res, [self = shared_from_this(), res](beast::error_code, std::size_t) { self->DoClose(); });
    }

}// namespace Awsmock::Agw
