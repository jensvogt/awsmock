#include <awsmock/agw/ProxyListener.h>

namespace Awsmock::Agw {

    ProxyListener::ProxyListener(net::io_context &ioc, const Dto::ApiGateway::ProxyConfig &cfg)
        : _ioc(ioc), _acceptor(net::make_strand(ioc)), _cfg(cfg) {

        beast::error_code ec;
        const auto address = net::ip::make_address(cfg.listenAddress);
        const tcp::endpoint endpoint{address, cfg.listenPort};

        if (_acceptor.open(endpoint.protocol(), ec); ec) {
            log_error << "open: " << ec.message();
            return;
        }
        if (_acceptor.set_option(net::socket_base::reuse_address(true), ec); ec) {
            log_error << "set_option reuse_address: " << ec.message();
            return;
        }
        if (_acceptor.bind(endpoint, ec); ec) {
            log_error << "bind to port " << cfg.listenPort << ": " << ec.message();
            return;
        }
        if (_acceptor.listen(net::socket_base::max_listen_connections, ec); ec) {
            log_error << "listen: " << ec.message();
            return;
        }
        log_info << "Proxy listener bound to " << cfg.listenAddress << ":" << cfg.listenPort;
    }

    void ProxyListener::Run() {
        net::dispatch(_acceptor.get_executor(),
                      beast::bind_front_handler(&ProxyListener::DoAccept, shared_from_this()));
    }

    void ProxyListener::DoAccept() {
        // Each accepted socket gets its own strand so sessions are thread-safe
        // without further locking.
        _acceptor.async_accept(net::make_strand(_ioc),
                               beast::bind_front_handler(&ProxyListener::OnAccept, shared_from_this()));
    }

    void ProxyListener::OnAccept(beast::error_code ec, tcp::socket socket) {
        if (ec) {
            if (ec != net::error::operation_aborted)
                log_error << "accept: " << ec.message();
        } else {
            beast::error_code opt_ec;
            socket.set_option(tcp::no_delay(true), opt_ec);
            if (opt_ec) log_warning << "TCP_NODELAY: " << opt_ec.message();
            std::make_shared<ProxySession>(_ioc, std::move(socket), _cfg)->Run();
        }
        DoAccept();
    }

}// namespace Awsmock::Agw
