//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewaySession.h>

namespace AwsMock::Service {

    GatewaySession::GatewaySession() {

        const Core::Configuration &configuration = Core::Configuration::instance();
        _queueLimit = configuration.GetValue<int>("awsmock.gateway.http.max-queue");
        _bodyLimit = configuration.GetValue<int>("awsmock.gateway.http.max-body");
        _timeout = configuration.GetValue<int>("awsmock.gateway.http.timeout");
        _verifySignature = configuration.GetValue<bool>("awsmock.aws.signature.verify");

        // Defaults for region and user; will be overwritten by authorized requests
        _region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        _user = Core::Configuration::instance().GetValue<std::string>("awsmock.user");
    };

    void GatewaySession::DoSession(boost::beast::tcp_stream &stream, const boost::asio::yield_context &yield) {
        boost::beast::error_code ec;

        // This buffer is required to persist across reads
        boost::beast::flat_buffer buffer;

        // This lambda is used to send messages
        for (;;) {

            // Set the timeout.
            stream.expires_after(std::chrono::seconds(_timeout));

            // Read a request
            http::request<http::dynamic_body> req;
            http::async_read(stream, buffer, req, yield[ec]);
            if (ec == http::error::end_of_stream)
                break;
            if (ec) {
                log_error << "read: " << ec.message();
                return;
            }

            // Handle the request
            http::message_generator msg = HandleRequest(std::move(req));

            // Determine if we should close the connection
            const bool keep_alive = msg.keep_alive();

            // Send the response
            boost::beast::async_write(stream, std::move(msg), yield[ec]);

            if (ec) {
                log_error << "write: " << ec.message();
                return;
            }

            if (!keep_alive) {
                // This means we should close the connection, usually because
                // the response indicated the "Connection: close" semantic.
                break;
            }
        }

        // Send a TCP shutdown
        ec = stream.socket().shutdown(ip::tcp::socket::shutdown_send, ec);
        log_trace << "Socket shutdown: " << ec.message();
        // At this point the connection is closed gracefully
    }

    // Return a response for the given request.
    //
    // The concrete type of the response message (which depends on the request) is type-erased in message_generator.
    template<class Body, class Allocator>
    http::message_generator GatewaySession::HandleRequest(http::request<Body, http::basic_fields<Allocator>> &&request) {

        // Make sure we can handle the method
        if (request.method() != http::verb::get && request.method() != http::verb::put &&
            request.method() != http::verb::post && request.method() != http::verb::delete_ &&
            request.method() != http::verb::head && request.method() != http::verb::connect &&
            request.method() != http::verb::options) {
            return Core::HttpUtils::BadRequest(request, "Unknown HTTP-method");
        }

        // Ping request
        if (request.method() == http::verb::connect) {
            log_debug << "Handle CONNECT request";
            Monitoring::MetricServiceTimer headTimer(GATEWAY_HTTP_TIMER, "method", "CONNECT");
            Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "CONNECT");
            return Core::HttpUtils::Ok(request);
        }

        // Request path must be absolute and not contain "..".
        if (request.target().empty() || request.target()[0] != '/' || request.target().find("..") != boost::beast::string_view::npos) {
            log_error << "Illegal request-target";
            return Core::HttpUtils::BadRequest(request, "Invalid target path");
        }

        // Handle OPTIONS requests
        if (request.method() == http::verb::options) {
            return HandleOptionsRequest(request);
        }

        std::shared_ptr<AbstractHandler> handler;
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            std::string target = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target");
            handler = GatewayRouter::instance().GetHandler(target);
            if (!handler) {
                log_error << "Handler not found, target: " << target;
                return Core::HttpUtils::BadRequest(request, "Handler not found");
            }
            log_trace << "Handler found, name: " << handler->name();

        } else {

            // Verify AWS signature
            if (_verifySignature && !Core::AwsUtils::VerifySignature(request, "none")) {
                log_warning << "AWS signature could not be verified";
                return Core::HttpUtils::Unauthorized(request, "AWS signature could not be verified");
            }

            // Get the module from the authorization key, or the target header field.
            Core::AuthorizationHeaderKeys authKey = GetAuthorizationKeys(request, {});

            _region = authKey.region;
            handler = GatewayRouter::instance().GetHandler(authKey.module);
            if (!handler) {
                log_error << "Handler not found, target: " << authKey.module;
                return Core::HttpUtils::BadRequest(request, "Handler not found");
            }
            log_trace << "Handler found, name: " << handler->name();
        }

        if (handler) {
            switch (request.method()) {
                case http::verb::get: {
                    Monitoring::MetricServiceTimer getTimer(GATEWAY_HTTP_TIMER, "method", "GET");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "GET");
                    return handler->HandleGetRequest(request, _region, _user);
                }
                case http::verb::put: {
                    Monitoring::MetricServiceTimer putTimer(GATEWAY_HTTP_TIMER, "method", "PUT");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "PUT");
                    return handler->HandlePutRequest(request, _region, _user);
                }
                case http::verb::post: {
                    Monitoring::MetricServiceTimer postTimer(GATEWAY_HTTP_TIMER, "method", "POST");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "POST");
                    return handler->HandlePostRequest(request, _region, _user);
                }
                case http::verb::delete_: {
                    Monitoring::MetricServiceTimer deleteTimer(GATEWAY_HTTP_TIMER, "method", "DELETE");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "DELETE");
                    return handler->HandleDeleteRequest(request, _region, _user);
                }
                case http::verb::head: {
                    Monitoring::MetricServiceTimer headTimer(GATEWAY_HTTP_TIMER, "method", "HEAD");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "HEAD");
                    return handler->HandleHeadRequest(request, _region, _user);
                }
                default:;
            }
        }
        return Core::HttpUtils::NotImplemented(request, "Not yet implemented");
    }

    Core::AuthorizationHeaderKeys GatewaySession::GetAuthorizationKeys(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey) {

        // Get signing version
        Core::AuthorizationHeaderKeys authKeys = {};

        if (const std::string authorizationHeader = request["Authorization"]; !authorizationHeader.empty()) {
            authKeys.signingVersion = Core::StringUtils::Split(authorizationHeader, ' ')[0];

            try {
                const boost::regex expr(R"(Credential=([a-zA-Z0-9]+)\/([0-9]{8})\/([a-zA-Z0-9\-]+)\/([a-zA-Z0-9\-]+)\/(aws4_request),\ ?SignedHeaders=(.*),\ ?Signature=(.*)$)");
                boost::smatch what;
                boost::regex_search(authorizationHeader, what, expr);
                authKeys.secretAccessKey = secretAccessKey.empty() ? "none" : secretAccessKey;
                authKeys.dateTime = what[2];
                authKeys.region = what[3];
                authKeys.module = what[4];
                authKeys.requestVersion = what[5];
                authKeys.signedHeaders = what[6];
                authKeys.signature = what[7];
                authKeys.scope = authKeys.dateTime + "/" + authKeys.region + "/" + authKeys.module + "/" + authKeys.requestVersion;
                authKeys.isoDateTime = Core::HttpUtils::GetHeaderValue(request, "x-amz-date");
                return authKeys;
            } catch (std::exception &e) {
                log_error << e.what();
            }

        } else if (Core::HttpUtils::HasHeader(request, "X-Amz-Target")) {

            if (Core::StringUtils::Contains(Core::HttpUtils::GetHeaderValue(request, "X-Amz-Target"), "Cognito")) {
                authKeys.module = "cognito-idp";
                authKeys.region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
            }

            return authKeys;
        }
        return {};
    }

    http::response<http::dynamic_body> GatewaySession::HandleOptionsRequest(const http::request<http::dynamic_body> &request) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "awsmock");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::allow, "*/*");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "*");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
        response.set(http::field::access_control_max_age, "86400");
        response.set(http::field::vary, "Accept-Encoding, Origin");
        response.set(http::field::keep_alive, "timeout=10, max=100");
        response.set(http::field::connection, "Keep-Alive");
        response.prepare_payload();

        // Send the response to the client
        return response;
    }

    void GatewaySession::HandleContinueRequest(boost::beast::tcp_stream &_stream) {
        http::response<http::empty_body> response;
        response.version(11);
        response.result(http::status::continue_);
        response.set(http::field::server, "awsmock");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::allow, "*/*");
        response.set(http::field::content_length, "0");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "*");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
        response.set(http::field::access_control_max_age, "86400");
        boost::beast::error_code ec;
        write(_stream, response, ec);
    }

}// namespace AwsMock::Service
