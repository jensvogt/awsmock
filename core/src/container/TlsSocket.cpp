//
// Created by vogje01 on 5/25/26.
//

// Boost includes
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/exception/diagnostic_information.hpp>

// AwsMock includes
#include <awsmock/core/container/TlsSocket.h>

namespace AwsMock::Core {

    TlsSocket::TlsSocket(const std::string &host, const int port, const std::string &caFile, const std::string &certFile, const std::string &keyFile, const bool verifyPeer)
        : DomainSocket(host + ":" + std::to_string(port)), _tlsHost(host), _tlsPort(port), _sslContext(boost::asio::ssl::context::tls_client) {

        _sslContext.set_verify_mode(verifyPeer ? boost::asio::ssl::verify_peer : boost::asio::ssl::verify_none);

        boost::system::error_code ec;
        if (!caFile.empty()) {
            ec = _sslContext.load_verify_file(caFile, ec);
            if (ec) {
                log_error << "Failed to load CA certificate, file: " << caFile << ", error: " << ec.message();
            }
        }
        if (!certFile.empty()) {
            ec = _sslContext.use_certificate_chain_file(certFile, ec);
            if (ec) {
                log_error << "Failed to load client certificate, file: " << certFile << ", error: " << ec.message();
            }
        }
        if (!keyFile.empty()) {
            ec = _sslContext.use_private_key_file(keyFile, boost::asio::ssl::context::pem, ec);
            if (ec) {
                log_error << "Failed to load client private key, file: " << keyFile << ", error: " << ec.message();
            }
        }
    }

    boost::beast::ssl_stream<boost::beast::tcp_stream> TlsSocket::Connect(boost::asio::io_context &ctx, boost::system::error_code &ec) const {

        boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ctx, const_cast<boost::asio::ssl::context &>(_sslContext));

        // Set SNI hostname so the server can select the right certificate
        if (!SSL_set_tlsext_host_name(stream.native_handle(), _tlsHost.c_str())) {
            ec.assign(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
            log_error << "Failed to set SNI hostname: " << ec.message();
            return stream;
        }

        boost::asio::ip::tcp::resolver resolver(ctx);
        const auto endpoints = resolver.resolve(_tlsHost, std::to_string(_tlsPort), ec);
        if (ec) {
            log_error << "DNS resolution of " << _tlsHost << ":" << _tlsPort << " failed: " << ec.message();
            return stream;
        }

        boost::beast::get_lowest_layer(stream).connect(endpoints, ec);
        if (ec) {
            log_error << "TCP connect to " << _tlsHost << ":" << _tlsPort << " failed: " << ec.message();
            return stream;
        }

        stream.handshake(boost::asio::ssl::stream_base::client, ec);
        if (ec) {
            log_error << "TLS handshake with " << _tlsHost << ":" << _tlsPort << " failed: " << ec.message();
        }
        return stream;
    }

    DomainSocketResult TlsSocket::SendJson(const verb method, const std::string &path) {
        return SendJson(method, path, {}, {});
    }

    DomainSocketResult TlsSocket::SendJson(const verb method, const std::string &path, const std::string &body) {
        return SendJson(method, path, body, {});
    }

    DomainSocketResult TlsSocket::SendJson(const verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {
        try {
            boost::asio::io_context ctx;
            boost::system::error_code ec;

            auto stream = Connect(ctx, ec);
            if (ec) {
                return {.statusCode = status::internal_server_error, .body = "TLS connect failed: " + ec.message()};
            }

            auto request = PrepareJsonMessage(method, path, body, headers);
            http::write(stream, request, ec);
            if (ec) {
                log_error << "TLS send failed, path: " << path << ", error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = "TLS send failed: " + ec.message()};
            }

            boost::beast::flat_buffer buffer;
            response_parser<string_body> parser;
            http::read(stream, buffer, parser, ec);
            if (ec) {
                log_error << "TLS read failed, path: " << path << ", error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = "TLS read failed: " + ec.message()};
            }

            // Graceful TLS shutdown — EOF after shutdown_notify is normal
            ec = stream.shutdown(ec);
            if (ec && ec != boost::asio::ssl::error::stream_truncated) {
                log_warning << "TLS shutdown warning, path: " << path << ", error: " << ec.message();
            }

            return PrepareResult(parser.release());

        } catch (boost::exception &e) {
            log_error << "TLS SendJson failed, path: " << path << ", error: " << diagnostic_information(e);
        }
        return {};
    }

    DomainSocketResult TlsSocket::SendBinary(const verb method, const std::string &path, const std::string &fileName) {
        return SendBinary(method, path, fileName, {});
    }

    DomainSocketResult TlsSocket::SendBinary(const verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) {
        try {
            boost::asio::io_context ctx;
            boost::system::error_code ec;

            auto stream = Connect(ctx, ec);
            if (ec) {
                return {.statusCode = status::internal_server_error, .body = "TLS connect failed: " + ec.message()};
            }

            // file_body message must be non-const so the serializer can read from the file
            auto request = PrepareBinaryMessage(method, path, fileName, headers);
            http::write(stream, request, ec);
            if (ec) {
                log_error << "TLS binary send failed, path: " << path << ", error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = "TLS binary send failed: " + ec.message()};
            }

            boost::beast::flat_buffer buffer;
            response<string_body> response;
            http::read(stream, buffer, response, ec);
            if (ec) {
                log_error << "TLS binary read failed, path: " << path << ", error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = "TLS binary read failed: " + ec.message()};
            }

            ec = stream.shutdown(ec);
            if (ec && ec != boost::asio::ssl::error::stream_truncated) {
                log_warning << "TLS shutdown warning, path: " << path << ", error: " << ec.message();
            }

            return PrepareResult(response);

        } catch (boost::exception &e) {
            log_error << "TLS SendBinary failed, path: " << path << ", error: " << diagnostic_information(e);
        }
        return {};
    }

    boost::asio::local::stream_protocol::socket TlsSocket::SendAttach(const verb method, const std::string &path, const std::map<std::string, std::string> &headers, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws) {

        boost::asio::io_context ctx;
        boost::asio::local::stream_protocol::socket placeholder(ctx);

        try {
            boost::system::error_code ec;

            auto stream = Connect(ctx, ec);
            if (ec) {
                log_error << "TLS SendAttach connect failed, path: " << path << ", error: " << ec.message();
                return placeholder;
            }

            request<empty_body> req;
            req.method(method);
            req.target(path);
            req.base().set(field::host, _tlsHost);
            req.set(field::user_agent, BOOST_BEAST_VERSION_STRING);
            for (const auto &[key, value]: headers) {
                req.base().set(key, value);
            }

            http::write(stream, req, ec);
            if (ec) {
                log_error << "TLS SendAttach write failed, path: " << path << ", error: " << ec.message();
                return placeholder;
            }

            boost::beast::flat_buffer buffer;
            response<dynamic_body> resp;
            http::read(stream, buffer, resp, ec);

            boost::asio::streambuf sb;
            sb.commit(boost::asio::buffer_copy(sb.prepare(resp.body().size()), resp.body().cdata()));
            std::istream lineStream(&sb);
            while (!lineStream.eof()) {
                std::string line;
                std::getline(lineStream, line);
                if (!line.empty()) {
                    ws.write(boost::asio::buffer(line));
                }
            }

            ec = stream.shutdown(ec);

        } catch (boost::exception &e) {
            log_error << "TLS SendAttach failed, path: " << path << ", error: " << diagnostic_information(e);
        }
        return placeholder;
    }

}// namespace AwsMock::Core
