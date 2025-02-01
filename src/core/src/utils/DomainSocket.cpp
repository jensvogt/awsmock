//
// Created by vogje01 on 5/28/24.
//

#include <awsmock/core/DomainSocket.h>

namespace AwsMock::Core {

    DomainSocketResult DomainSocket::SendJson(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) const {

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

    DomainSocketResult DomainSocket::SendBinary(verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) const {

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

    request<string_body> DomainSocket::PrepareJsonMessage(const verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        request<string_body> request;

        request.method(method);
        request.target(path);
        request.body() = body;
        request.prepare_payload();
        request.base().set(field::host, "localhost");
        request.base().set(field::content_type, "application/json");
        request.base().set(field::content_length, std::to_string(body.size()));

        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                request.base().set(fst, snd);
            }
        }
        return request;
    }

    request<file_body> DomainSocket::PrepareBinaryMessage(const verb method, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;
        request<file_body> request;
        request.method(method);
        request.target(path);
        request.base().set("Host", "localhost");
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.body().open(filename.c_str(), boost::beast::file_mode::scan, ec);
        request.prepare_payload();

        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                request.base().set(fst, snd);
            }
        }
        return request;
    }

    DomainSocketResult DomainSocket::PrepareResult(response<string_body> response) {

        DomainSocketResult domainSocketResult;
        domainSocketResult.body = response.body();
        domainSocketResult.statusCode = response.result();
        return domainSocketResult;
    }

}// namespace AwsMock::Core
