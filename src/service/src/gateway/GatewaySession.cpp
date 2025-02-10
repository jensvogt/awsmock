//
// Created by vogje01 on 5/27/24.
//

#include <asio/buffers_iterator.hpp>
#include <awsmock/service/gateway/GatewaySession.h>

namespace AwsMock::Service {

    GatewaySession::GatewaySession(ip::tcp::socket &&socket) : _stream(std::move(socket)) {

        const Core::Configuration &configuration = Core::Configuration::instance();
        _queueLimit = configuration.GetValueInt("awsmock.gateway.http.max-queue");
        _bodyLimit = configuration.GetValueInt("awsmock.gateway.http.max-body");
        _timeout = configuration.GetValueInt("awsmock.gateway.http.timeout");
        _verifySignature = configuration.GetValueBool("awsmock.aws.signature.verify");
    };

    void GatewaySession::Run() {
        dispatch(_stream.get_executor(), boost::beast::bind_front_handler(&GatewaySession::DoRead, shared_from_this()));
    }

    void GatewaySession::DoRead() {

        // Construct a new parser for each message
        _parser.emplace();
        _buffer.clear();

        // Apply a reasonable limit to the allowed size
        // of the body in bytes to prevent abuse.
        _parser->body_limit(boost::none);

        // Set the timeout.
        _stream.expires_after(std::chrono::seconds(_timeout));

        // Read a request using the parser-oriented interface
        http::async_read(_stream, _buffer, *_parser, boost::beast::bind_front_handler(&GatewaySession::OnRead, shared_from_this()));
    }

    void GatewaySession::OnRead(const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream) {
            return DoShutdown();
        }

        if (ec) {
            return;
        }

        // Read the header
        boost::beast::error_code ev;
        read_header(_stream, _buffer, *_parser, ev);
        if (ec)
            return;

        // Handle 100-continue requests
        if (boost::beast::iequals(_parser->get()[http::field::expect], "100-continue")) {
            HandleContinueRequest(_stream);
        }

        //Core::HttpUtils::DumpHeaders(_parser.get().get());

        // Read the rest of the request.
        // if (boost::beast::iequals(_parser->get()[http::field::transfer_encoding], "chunked")) {
        //     boost::beast::error_code Ev;
        //     PrintChunkedBody(std::cerr, _stream, _buffer, Ev);
        // } else {
        read(_stream, _buffer, *_parser, ev);
        //}
        // Read the rest of the request.
        //        read(_stream, _buffer, *_parser, ev);

        // Send the response
        QueueWrite(HandleRequest(_parser->release()));

        // If we aren't at the queue limit, try to pipeline another request
        if (_response_queue.size() < _queueLimit) {
            DoRead();
        }
        log_trace << "Request queue size: " << _response_queue.size() << " limit: " << _queueLimit;
    }

    void GatewaySession::QueueWrite(http::message_generator response) {

        // Allocate and store the work
        _response_queue.push(std::move(response));

        // If there was no previous work, start the write loop
        if (_response_queue.size() == 1)
            DoWrite();
    }

    // Return a response for the given request.
    //
    // The concrete type of the response message (which depends on the
    // request), is type-erased in message_generator.
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
        std::string region = Core::Configuration::instance().GetValueString("awsmock.region");
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            std::string target = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target");
            handler = GatewayRouter::instance().GetHandler(target);
            log_trace << "Handler found, name: " << handler->name();

        } else {

            // Verify AWS signature
            if (_verifySignature && !Core::AwsUtils::VerifySignature(request, "none")) {
                log_warning << "AWS signature could not be verified";
                return Core::HttpUtils::Unauthorized(request, "AWS signature could not be verified");
            }

            // Get the module from the authorization key, or the target header field.
            Core::AuthorizationHeaderKeys authKey = GetAuthorizationKeys(request, {});

            region = authKey.region;
            handler = GatewayRouter::instance().GetHandler(authKey.module);
            log_trace << "Handler found, name: " << handler->name();
        }

        if (handler) {
            switch (request.method()) {
                case http::verb::get: {
                    Monitoring::MetricServiceTimer getTimer(GATEWAY_HTTP_TIMER, "method", "GET");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "GET");
                    return handler->HandleGetRequest(request, region, "none");
                }
                case http::verb::put: {
                    Monitoring::MetricServiceTimer putTimer(GATEWAY_HTTP_TIMER, "method", "PUT");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "PUT");
                    return handler->HandlePutRequest(request, region, "none");
                }
                case http::verb::post: {
                    Monitoring::MetricServiceTimer postTimer(GATEWAY_HTTP_TIMER, "method", "POST");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "POST");
                    return handler->HandlePostRequest(request, region, "none");
                }
                case http::verb::delete_: {
                    Monitoring::MetricServiceTimer deleteTimer(GATEWAY_HTTP_TIMER, "method", "DELETE");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "DELETE");
                    return handler->HandleDeleteRequest(request, region, "none");
                }
                case http::verb::head: {
                    Monitoring::MetricServiceTimer headTimer(GATEWAY_HTTP_TIMER, "method", "HEAD");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "HEAD");
                    return handler->HandleHeadRequest(request, region, "none");
                }
                default:;
            }
        }
        return Core::HttpUtils::NotImplemented(request, "Not yet implemented");
    }

    // Called to start/continue the write-loop. Should not be called when write_loop is already active.
    void GatewaySession::DoWrite() {
        if (!_response_queue.empty()) {
            bool keep_alive = _response_queue.front().keep_alive();
            boost::beast::async_write(_stream,
                                      std::move(_response_queue.front()),
                                      boost::beast::bind_front_handler(&GatewaySession::OnWrite,
                                                                       shared_from_this(),
                                                                       keep_alive));
        }
    }

    void GatewaySession::OnWrite(const bool keep_alive, const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream)
            return DoShutdown();

        if (!keep_alive) {
            // This means we should close the connection, usually because the response indicated the "Connection: close" semantic.
            log_debug << "Connection shutdown";
            return DoShutdown();
        }

        // Resume the read if it has been paused
        if (_response_queue.size() == _queueLimit)
            DoRead();

        _response_queue.pop();

        DoWrite();
    }

    void GatewaySession::DoShutdown() {

        // Send a TCP shutdown
        boost::beast::error_code ec;
        _stream.socket().shutdown(ip::tcp::socket::shutdown_send, ec);
        // At this point the connection is closed gracefully
    }

    void GatewaySession::DoClose() {
        // Send a TCP shutdown
        boost::beast::error_code ec;
        _stream.socket().close(ec);
        // At this point the connection is closed gracefully
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
                authKeys.region = Core::Configuration::instance().GetValueString("awsmock.region");
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
