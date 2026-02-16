
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
        log_error << "Real method not implemented";
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

    http::response<http::dynamic_body> AbstractHandler::SendResponse(const http::request<http::dynamic_body> &request, const http::status &status, const std::string &fileName, long contentLength, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(status);
        response.keep_alive(request.keep_alive());
        response.set(http::field::connection, "close");
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::content_length, std::to_string(contentLength));
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
        response.set(http::field::connection, "keep-alive");
        response.set(http::field::keep_alive, "timeout=5, max=1000");

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

    http::response<http::dynamic_body> AbstractHandler::SendResponse(const http::request<http::dynamic_body> &request, const std::string &fileName, long min, long max, long size, long totalSize, const http::status &status, const std::map<std::string, std::string> &headers) {
        log_debug << "Sending OK response, state: 200, filename: " << fileName << " min: " << min << " max: " << max << " size: " << size;

        try {

            // Prepare the response message
            http::response<http::dynamic_body> response;
            response.version(request.version());
            response.keep_alive(request.keep_alive());
            response.result(http::status::partial_content);
            response.set(http::field::connection, "close");
            response.set(http::field::server, "awsmock");
            response.set(http::field::content_type, "application/octet-stream");
            response.set(http::field::accept_ranges, "bytes");
            response.set(http::field::content_range, "bytes " + std::to_string(min) + "-" + std::to_string(max) + "/" + std::to_string(totalSize));
            response.set(http::field::content_length, std::to_string(size));
            response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
            response.set(http::field::access_control_allow_origin, "*");
            response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
            response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
            response.set(http::field::connection, "keep-alive");
            response.set(http::field::keep_alive, "timeout=5, max=1000");

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
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

    http::response<http::dynamic_body> AbstractHandler::SendResponse(const http::request<http::dynamic_body> &request, const http::status &status, const std::string &body, const std::map<std::string, std::string> &headers) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(status);
        response.keep_alive(request.keep_alive());
        response.set(http::field::connection, "close");
        response.set(http::field::server, "awsmock");
        response.set(http::field::content_type, "application/json");
        response.set(http::field::content_length, std::to_string(body.length()));
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::last_modified, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "cache-control,content-type,x-amz-target,x-amz-user-agent");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
        response.set(http::field::connection, "keep-alive");
        response.set(http::field::keep_alive, "timeout=5, max=1000");

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

}// namespace AwsMock::Service
