//
// Created by vogje01 on 5/28/24.
//

#include "awsmock/core/FileUtils.h"


#include <awsmock/core/UnixSocket.h>
#include <boost/asio/streambuf.hpp>
#include <boost/exception/detail/exception_ptr.hpp>
#include <boost/json/detail/buffer.hpp>
#include <boost/json/detail/stream.hpp>

namespace AwsMock::Core {

    DomainSocketResult UnixSocket::SendJson(const verb method, const std::string &path) {
        return SendJson(method, path, {});
    }

    DomainSocketResult UnixSocket::SendJson(const verb method, const std::string &path, const std::string &body) {
        return SendJson(method, path, body, {});
    }

    DomainSocketResult UnixSocket::SendJson(verb method, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        try {
            boost::asio::io_context ctx;
            boost::system::error_code ec;

            // Connect to peer
            boost::asio::local::stream_protocol::endpoint endpoint(_basePath);
            boost::asio::local::stream_protocol::socket socket(ctx);
            ec = socket.connect(endpoint, ec);
            if (ec) {
                log_error << "Could not connect to docker UNIX domain socket, path: " << path << ", method: " << method << ", error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = "Could not connect to docker UNIX domain socket, error: " + ec.message()};
            }

            // Prepare the message
            request<string_body> request = PrepareJsonMessage(method, path, body, headers);

            // Write to unix socket
            std::ostringstream oss;
            oss << request;
            boost::asio::write(socket, boost::asio::buffer(oss.str()), boost::asio::transfer_all());

            boost::beast::flat_buffer buffer;
            response_parser<string_body> parser;
            parser.body_limit(8 * 1024 * 1024);

            http::read(socket, buffer, parser, ec);
            if (ec) {
                log_error << "Read from docker daemon failed, error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = "Read from docker daemon failed, error: " + ec.message()};
            }
            socket.close();

            return PrepareResult(parser.release());
        } catch (boost::exception &e) {
            log_error << "Send JSON message failed, path: " << path << ", method: " << method << ", error: " << diagnostic_information(e);
        }
        return {};
    }

    DomainSocketResult UnixSocket::SendBinary(const verb method, const std::string &path, const std::string &fileName) {
        return SendBinary(method, path, fileName, {});
    }

    DomainSocketResult UnixSocket::SendBinary(verb method, const std::string &path, const std::string &fileName, const std::map<std::string, std::string> &headers) {
        try {

            boost::system::error_code ec;
            boost::asio::io_context ctx;

            // Connect to peer
            boost::asio::local::stream_protocol::endpoint endpoint(_basePath);
            boost::asio::local::stream_protocol::socket socket(ctx);
            ec = socket.connect(endpoint, ec);
            if (ec) {
                log_error << "Could not connect to docker UNIX domain socket, basePath: " << _basePath << ", method: " << method << ", error: " << ec.message();
                return {.statusCode = status::internal_server_error, .body = "Could not connect to docker UNIX domain socket"};
            }

            // Prepare the message
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

        } catch (boost::exception &e) {
            log_error << "Send binary failed, path: " << path << ", method: " << method << ", error: " << diagnostic_information(e);
        }
        return {};
    }

    boost::asio::local::stream_protocol::socket UnixSocket::SendAttach(verb method, const std::string &path, const std::map<std::string, std::string> &headers, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws) {

        boost::asio::io_context ctx;
        boost::asio::local::stream_protocol::endpoint endpoint(_basePath);
        boost::asio::local::stream_protocol::socket socket(ctx);

        try {
            boost::system::error_code ec;
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

            timeval timeout{};
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
#ifdef WIN32
            setsockopt(socket.native_handle(), SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
            setsockopt(socket.native_handle(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
            // Write to unix socket
            http::write(socket, request);

            boost::beast::flat_buffer buffer;
            response<dynamic_body> response;

            boost::beast::net::streambuf sb;
            sb.commit(boost::beast::net::buffer_copy(sb.prepare(response.body().size()), response.body().cdata()));
            std::istream stream(&sb);
            while (!stream.eof()) {
                std::string line;
                std::getline(stream, line);
                log_info << "Read " << line;
                ws.write(boost::asio::buffer(line));
            }
            log_info << "Attached to container finished";
            return socket;

        } catch (boost::exception &e) {
            log_error << "Attach to container failed, path: " << path << ", method: " << method << ", error: " << diagnostic_information(e);
        }
        return socket;
    }

}// namespace AwsMock::Core
