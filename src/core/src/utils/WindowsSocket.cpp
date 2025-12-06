//
// Created by vogje01 on 5/28/24.
//

#include <awsmock/core/WindowsSocket.h>

namespace AwsMock::Core {
    DomainSocketResult WindowsSocket::SendJson(const verb method, const std::string &path) {
        return SendJson(method, path, {}, {});
    }

    DomainSocketResult WindowsSocket::SendJson(const verb method, const std::string &path, const std::string &body) {
        return SendJson(method, path, body, {});
    }

    DomainSocketResult WindowsSocket::SendJson(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {
        boost::asio::io_context ctx;

        boost::asio::ip::tcp::resolver resolver(ctx);
        boost::beast::tcp_stream stream(ctx);

        // Resolve host/port
        try {
            boost::system::error_code ec;
            std::string host = "localhost";
            int port = 2375;
            GetHostPort(_basePath, host, port);
            auto const results = resolver.resolve(host, std::to_string(port));

            // Connect
            stream.connect(results, ec);
            if (ec) {
                log_error << "Connect to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }

            // Prepare message
            request<string_body> request = PrepareJsonMessage(method, path, body, {});

            // Write to TCP socket
            http::write(stream, request, ec);
            if (ec) {
                log_error << "Send to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }

            boost::beast::flat_buffer buffer;
            response<string_body> response;

            http::read(stream, buffer, response);
            if (ec) {
                log_error << "Read from " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }

            // Cleanup
            ec = stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            if (ec) {
                log_error << "Shutdown socket failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }
            return PrepareResult(response);
        } catch (const boost::system::system_error &exc) {
            log_error << "Error sending JSON message, error: " << exc.what();
        }
        return {.statusCode = status::internal_server_error, .body = "Generell error"};
    }

    DomainSocketResult WindowsSocket::SendBinary(const verb method, const std::string &path, const std::string &fileName) {
        return SendBinary(method, path, fileName, {});
    }

    DomainSocketResult WindowsSocket::SendBinary(verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) {

        boost::asio::io_context ctx;

        boost::asio::ip::tcp::resolver resolver(ctx);
        boost::beast::tcp_stream stream(ctx);

        // Resolve host/port
        try {
            boost::system::error_code ec;
            std::string host = "localhost";
            int port = 2375;
            GetHostPort(_basePath, host, port);
            auto const results = resolver.resolve(host, std::to_string(port));

            // Connect
            stream.connect(results, ec);
            if (ec) {
                log_error << "Connect to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }

            // Prepare the message
            request<file_body> request = PrepareBinaryMessage(method, path, fileName, {});

            // Write to TCP socket
            http::write(stream, request, ec);
            if (ec) {
                log_error << "Send to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }

            boost::beast::flat_buffer buffer;
            response<string_body> response;

            http::read(stream, buffer, response);
            if (ec) {
                log_error << "Read from " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }

            // Cleanup
            ec = stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            if (ec) {
                log_error << "Shutdown socket failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = ec.message()};
            }
            return PrepareResult(response);
        } catch (const boost::system::system_error &exc) {
            log_error << "Error sending JSON message, error: " << exc.what();
        }
        return {.statusCode = status::internal_server_error, .body = "General error"};
    }

    boost::asio::local::stream_protocol::socket WindowsSocket::SendAttach(verb method, const std::string &path, const std::map<std::string, std::string> &headers, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws) {

        boost::system::error_code ec;

        boost::asio::io_context ctx;
        boost::asio::local::stream_protocol::endpoint endpoint(_basePath);
        boost::asio::local::stream_protocol::socket socket(ctx);
        ec = socket.connect(endpoint, ec);
        if (ec) {
            log_error << "Could not connect to docker UNIX domain socket, basePath: " << _basePath << ", method: " << method << ", error: " << ec.message();
            return socket;
        }

        // Prepare the message
        request<empty_body> request;
        request.method(method);
        request.target(path);
        request.base().set("Host", "localhost");
        request.set(field::user_agent, BOOST_BEAST_VERSION_STRING);

        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                request.base().set(fst, snd);
            }
        }

        // Write to unix socket
        http::write(socket, request);

        boost::beast::flat_buffer buffer;
        response<string_body> response;
        read(socket, buffer, response, ec);
        if (ec) {
            log_error << "Send to docker daemon failed, error: " << ec.message();
        }
        socket.close();
        if (ec) {
            log_error << "Shutdown socket failed, error: " << ec.message();
        }
        return socket;
    }

    void WindowsSocket::GetHostPort(const std::string &url, std::string &host, int &port) {
        const boost::regex expr(R"(.*:\/\/(.*):(\d+)$)");
        boost::smatch what;
        boost::regex_search(url, what, expr);
        host = what[1];
        port = std::stoi(what[2]);
    }

}// namespace AwsMock::Core
