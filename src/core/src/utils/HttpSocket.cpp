//
// Created by vogje01 on 5/28/24.
//

#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/HttpSocket.h>

namespace AwsMock::Core {

    HttpSocketResponse HttpSocket::SendJson(http::verb method, const std::string &host, int port, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        boost::beast::tcp_stream stream(ctx);

        try {

            // Resolve host/port
            boost::system::error_code ec;
            auto const results = resolver.resolve(host, std::to_string(port));

            // Connect
            stream.connect(results, ec);
            stream.socket().set_option(boost::asio::ip::tcp::socket::keep_alive(false));
            if (ec) {
                log_error << "Connect to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            // Prepare the message
            http::request<http::string_body> request = PrepareJsonMessage(method, host, path, body, headers);

            // Write to TCP socket
            http::write(stream, request, ec);
            if (ec) {
                log_error << "Send to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            // Read the response
            boost::beast::flat_buffer buffer;
            http::response<http::string_body> response;
            http::read(stream, buffer, response);
            if (ec) {
                log_error << "Read from " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            // Cleanup
            ec = stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            if (ec) {
                log_warning << "Shutdown socket failed, error: " << ec.message();
            }
            return PrepareResult(response);

        } catch (const boost::exception &e) {
            log_error << "Send JSON message failed, host:" << host << ", port: " << port;
        }
        return {.statusCode = http::status::internal_server_error};
    }

    HttpSocketResponse HttpSocket::SendAuthorizedJson(http::verb method, const std::string &module, const std::string &host, int port, const std::string &path, const std::string &signedHeaders, std::map<std::string, std::string> &headers, const std::string &body) {

        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        boost::beast::tcp_stream stream(ctx);

        try {

            // Resolve host/port
            boost::system::error_code ec;
            const auto result = resolver.resolve(host, std::to_string(port));

            // Connect
            stream.connect(result, ec);
            if (ec) {
                log_error << "Connect to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            // Prepare the message
            http::request<http::string_body> request = PrepareJsonMessage(method, host, path, body, headers);
            AwsUtils::AddAuthorizationHeader(to_string(method), path, headers, module, "application/x-amz-json-1.0", signedHeaders, body);
            for (const auto &[fst, snd]: headers) {
                request.set(fst, snd);
            }

            // Write to TCP socket
            http::write(stream, request, ec);
            if (ec) {
                log_error << "Send to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            // Read the response
            boost::beast::flat_buffer buffer;
            http::response<http::string_body> response;
            http::read(stream, buffer, response);
            if (ec) {
                log_error << "Read from " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            // Cleanup
            ec = stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            if (ec) {
                log_warning << "Shutdown socket failed, error: " << ec.message();
            }
            return PrepareResult(response);

        } catch (const boost::exception &e) {
            log_error << "Send authorized message failed, host: " << host << ", port: " << port << ", error: " << diagnostic_information(e);
        }
        return {.statusCode = http::status::internal_server_error};
    }

    HttpSocketResponse HttpSocket::SendBinary(http::verb method, const std::string &host, int port, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers) {

        boost::asio::io_context ctx;
        boost::asio::ip::tcp::resolver resolver(ctx);
        boost::beast::tcp_stream stream(ctx);

        try {
            // Prepare the message
            http::request<http::file_body> request = PrepareBinaryMessage(method, path, filename, headers);

            // Write to socket
            boost::system::error_code ec;
            http::write(stream, request);
            if (ec) {
                log_error << "Send to " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            // Read the response
            boost::beast::flat_buffer buffer;
            http::response<http::dynamic_body> response;
            read(stream, buffer, response, ec);
            if (ec) {
                log_error << "Read from " << host << ":" << port << " failed, error: " << ec.message();
                return {.statusCode = http::status::internal_server_error, .body = ec.message()};
            }

            ec = stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            if (ec) {
                log_warning << "Shutdown socket failed, error: " << ec.message();
            }
            return PrepareResult(response);

        } catch (const boost::exception &e) {
            log_error << "Send binary message failed, host: " << host << ", port: " << port << ", error: " << diagnostic_information(e);
        }
        return {.statusCode = http::status::internal_server_error};
    }

    http::request<http::string_body> HttpSocket::PrepareJsonMessage(const http::verb method, const std::string &host, const std::string &path, const std::string &body, const std::map<std::string, std::string> &headers) {

        http::request<http::string_body> request;

        request.method(method);
        request.set(http::field::host, host);
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.set(http::field::content_type, "application/json");
        request.set(http::field::date, DateTimeUtils::HttpFormatNow());
        request.set(http::field::content_length, std::to_string(body.size()));
        request.set(http::field::connection, "close");
        request.target(path);
        request.body() = body;
        request.prepare_payload();

        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                request.base().set(fst, snd);
            }
        }
        return request;
    }

    http::request<http::file_body> HttpSocket::PrepareBinaryMessage(const http::verb method, const std::string &path, const std::string &filename, const std::map<std::string, std::string> &headers) {

        boost::system::error_code ec;
        http::request<http::file_body> request;
        request.method(method);
        request.target(path);
        request.base().set("Host", "localhost");
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.set(http::field::content_type, "application/octet-stream");
        request.set(http::field::date, DateTimeUtils::HttpFormatNow());
        request.set(http::field::connection, "close");
        request.body().open(filename.c_str(), boost::beast::file_mode::scan, ec);
        request.prepare_payload();

        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                request.base().set(fst, snd);
            }
        }
        return request;
    }

    HttpSocketResponse HttpSocket::PrepareResult(http::response<http::string_body> response) {
        HttpSocketResponse res = {.statusCode = response.result(), .body = response.body()};
        for (const auto &header: response) {
            res.headers[header.name_string()] = header.value();
        }
        return res;
    }

    HttpSocketResponse HttpSocket::PrepareResult(http::response<http::dynamic_body> response) {
        boost::beast::net::streambuf sb;
        sb.commit(boost::beast::net::buffer_copy(sb.prepare(response.body().size()), response.body().cdata()));

        return {.statusCode = response.result(), .body = boost::beast::buffers_to_string(sb.data())};
    }

}// namespace AwsMock::Core
