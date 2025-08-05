
#include <awsmock/service/common/AbstractHandler.h>

namespace AwsMock::Service {
    http::response<http::dynamic_body> AbstractHandler::HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_error << "Real method not implemented";
        return {};
    }

    http::response<http::dynamic_body> AbstractHandler::HandlePutRequest(http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_error << "Real method not implemented";
        return {};
    }

    http::response<http::dynamic_body> AbstractHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        //        log_error << "Real method not implemented";
        return {};
    }

    http::response<http::dynamic_body> AbstractHandler::HandlePostRequest(const boost::beast::tcp_stream &stream, const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        //log_error << "Real method not implemented";
        return {};
    }

    http::response<http::dynamic_body> AbstractHandler::HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_error << "Real method not implemented";
        return {};
    }

    http::response<http::dynamic_body> AbstractHandler::HandleHeadRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_error << "Real method not implemented";
        return {};
    }

    http::response<http::dynamic_body> AbstractHandler::SendOkResponse(const http::request<http::dynamic_body> &request, const std::string &body, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::content_length, "0");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }

        // Body
        if (!body.empty()) {
            ostream(response.body()).write(body.c_str(), body.length()).flush();
            response.prepare_payload();
        }

        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendOkResponse(const http::request<http::dynamic_body> &request, const std::string &fileName, long contentLength, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::content_length, std::to_string(contentLength));
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Body
        std::ifstream ifs(fileName);
        ostream(response.body()) << ifs.rdbuf();
        ifs.close();
        response.prepare_payload();

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }
        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendNoContentResponse(const http::request<http::dynamic_body> &request, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::no_content);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }

        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendInternalServerError(const http::request<http::dynamic_body> &request, const std::string &body, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::internal_server_error);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Body
        if (!body.empty()) {
            ostream(response.body()).write(body.c_str(), body.size());
        }
        response.prepare_payload();

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }

        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendBadRequestError(const http::request<http::dynamic_body> &request, const std::string &body, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::bad_request);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Body
        if (!body.empty()) {
            ostream(response.body()).write(body.c_str(), body.size());
        }
        response.prepare_payload();

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }

        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendNotFoundError(const http::request<http::dynamic_body> &request, const std::string &body, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::not_found);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Body
        if (!body.empty()) {
            ostream(response.body()).write(body.c_str(), body.size());
        }
        response.prepare_payload();

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }

        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendRangeResponse(const http::request<http::dynamic_body> &request, const std::string &fileName, long min, long max, long size, long totalSize, const http::status &status, const std::map<std::string, std::string> &headers) {
        log_debug << "Sending OK response, state: 200, filename: " << fileName << " min: " << min << " max: " << max << " size: " << size;

        try {

            // Prepare the response message
            http::response<http::dynamic_body> response;
            response.version(request.version());
            response.result(http::status::partial_content);
            response.set(http::field::server, "awsmock");
            response.set(http::field::content_type, "application/octet-stream");
            response.set(http::field::accept_ranges, "bytes");
            response.set(http::field::content_range, "bytes " + std::to_string(min) + "-" + std::to_string(max) + "/" + std::to_string(totalSize));
            response.set(http::field::content_length, std::to_string(size));
            response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
            response.set(http::field::access_control_allow_origin, "*");
            response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
            response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

            // Body is part of file from min -> max
            auto buffer = new char[size];
            std::ifstream file(fileName.c_str(), std::ios::binary);
            file.seekg(min, std::ios::beg);
            file.read(buffer, size);
            int count = file.gcount();
            ostream(response.body()).write(buffer, size);
            response.prepare_payload();
            file.close();
            delete[] buffer;

            // Copy headers
            if (!headers.empty()) {
                for (const auto &[fst, snd]: headers) {
                    response.set(fst, snd);
                }
            }

            // Send the response to the client
            log_debug << "Range response finished, filename: " << fileName << " size: " << count << " status: " << status;
            return response;

        } catch (std::exception &exc) {
            log_error << exc.what();
            return SendInternalServerError(request, exc.what());
        }
    }

    http::response<http::dynamic_body> AbstractHandler::SendHeadResponse(const http::request<http::dynamic_body> &request, const long contentLength, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::content_length, std::to_string(contentLength));
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }

        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendContinueResponse(const http::request<http::dynamic_body> &request) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::continue_);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Send the response to the client
        return response;
    }

    http::response<http::dynamic_body> AbstractHandler::SendResponse(const http::request<http::dynamic_body> &request, http::status status, const std::string &body, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(status);
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");

        // Copy headers
        if (!headers.empty()) {
            for (const auto &[fst, snd]: headers) {
                response.set(fst, snd);
            }
        }

        // Body
        if (!body.empty()) {
            ostream(response.body()).write(body.c_str(), body.length()).flush();
            response.prepare_payload();
        }

        // Send the response to the client
        return response;
    }

    // Called to start/continue the write-loop. Should not be called when write_loop is already active.
    void AbstractHandler::DoWrite(http::message_generator response) {
        bool keep_alive = false;
        boost::beast::async_write(_stream,
                                  std::move(response),
                                  boost::beast::bind_front_handler(&AbstractHandler::OnWrite,
                                                                   shared_from_this(),
                                                                   keep_alive));
    }

    void AbstractHandler::OnWrite(const bool keep_alive, const boost::beast::error_code &ec, std::size_t bytes_transferred) const {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream)
            return DoShutdown();
    }

    void AbstractHandler::DoShutdown() const {

        // Send a TCP shutdown
        boost::beast::error_code ec;
        ec = _stream.socket().shutdown(ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            log_error << "Backend stream shutdown failed: " << ec.message();
            ec = _stream.socket().close(ec);
            if (ec) {
                log_error << "Close failed: " << ec.message();
            }
        }
        // At this point the connection is closed gracefully
    }
}// namespace AwsMock::Service
