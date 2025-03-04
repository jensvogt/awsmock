//
// Created by vogje01 on 5/28/24.
//

#include <awsmock/core/UnixSocket.h>

namespace AwsMock::Core {

    DomainSocketResult UnixSocket::SendJson(verb method, const std::string &path) {
        return SendJson(method, path, {});
    }

    DomainSocketResult UnixSocket::SendJson(verb method, const std::string &path, const std::string &body) {
        return SendJson(method, path, body, {});
    }

    DomainSocketResult UnixSocket::SendJson(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;

        boost::asio::io_context ctx;
        boost::asio::local::stream_protocol::endpoint endpoint(_path);
        boost::asio::local::stream_protocol::socket socket(ctx);
        ec = socket.connect(endpoint, ec);
        if (ec) {
            log_error << "Could not connect to docker UNIX domain socket, error: " << ec.message();
            return {.statusCode = status::internal_server_error, .body = "Could not connect to docker UNIX domain socket, error: " + ec.message()};
        }

        // Prepare message
        request<string_body> request = PrepareJsonMessage(method, path, body, headers);

        // Write to unix socket
        std::ostringstream oss;
        oss << request;
        boost::asio::write(socket, boost::asio::buffer(oss.str()), boost::asio::transfer_all());

        boost::beast::flat_buffer buffer;
        response<string_body> response;
        read(socket, buffer, response, ec);
        if (ec) {
            log_error << "Read from docker daemon failed, error: " << ec.message();
            return {.statusCode = status::internal_server_error, .body = "Read from docker daemon failed, error: " + ec.message()};
        }
        socket.close();
        return PrepareResult(response);
    }

    DomainSocketResult UnixSocket::SendBinary(verb method, const std::string &path, const std::string &fileName) {
        return SendBinary(method, fileName, {});
    }
    
    DomainSocketResult UnixSocket::SendBinary(verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;

        boost::asio::io_context ctx;
        boost::asio::local::stream_protocol::endpoint endpoint(_path);
        boost::asio::local::stream_protocol::socket socket(ctx);
        ec = socket.connect(endpoint, ec);
        if (ec) {
            log_error << "Could not connect to docker UNIX domain socket";
            return {.statusCode = http::status::internal_server_error, .body = "Could not connect to docker UNIX domain socket"};
        }

        // Prepare message
        request<file_body> request = PrepareBinaryMessage(method, path, fileName, headers);

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
            return {.statusCode = status::internal_server_error, .body = ec.message()};
        }
        return PrepareResult(response);
    }

}// namespace AwsMock::Core
