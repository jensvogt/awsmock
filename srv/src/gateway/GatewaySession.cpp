//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewaySession.h>

namespace AwsMock::Service {
    GatewaySession::GatewaySession(boost::asio::io_context &ioc, ip::tcp::socket &&socket) : _ioc(ioc), _stream(std::move(socket)), _queue(*this) {
        const Core::Configuration &configuration = Core::Configuration::instance();
        _queueLimit = configuration.GetValue<int>("awsmock.gateway.http.max-queue");
        _bodyLimit = configuration.GetValue<int>("awsmock.gateway.http.max-body");
        _timeout = configuration.GetValue<int>("awsmock.gateway.http.timeout");
        _verifySignature = configuration.GetValue<bool>("awsmock.aws.signature.verify");

        // Defaults for region and user; will be overwritten by authorized requests
        _region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        _user = Core::Configuration::instance().GetValue<std::string>("awsmock.user");
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
        _parser->body_limit(_bodyLimit);

        // Set the timeout.
        _stream.expires_after(std::chrono::seconds(_timeout));

        // Read a request using the parser-oriented interface
        http::async_read(_stream, _buffer, *_parser, boost::beast::bind_front_handler(&GatewaySession::OnRead, shared_from_this()));
    }

    void GatewaySession::OnRead(const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream)
            return DoClose();

        if (ec) {
            log_error << "Read failed: " << ec.message();
            _buffer.commit(90000000);
            std::cout << "--- ERROR BUFFER DUMP (" << _buffer.size() << " bytes) ---" << std::endl;
            std::cout << boost::beast::buffers_to_string(_buffer.cdata()) << std::endl;
            std::cout << "\n------------------------------------------" << std::endl;
            return;
        }

        // Data is already in _parser! No need for http::read(ev).
        // Handle 100-continue (using the data already in the parser)
        if (boost::beast::iequals(_parser->get()[http::field::expect], "100-continue")) {
            HandleContinueRequest(_stream, _parser->get());
            DoRead();
        }

        // Process the request that was just finished by the async operation
        QueueWrite(HandleRequest(_parser->release()));

        // If we aren't at the queue limit, try to pipeline another request
        if (_response_queue.size() < _queueLimit)
            DoRead();
    }

    // Return a response for the given request. The concrete type of the response message (which depends on the request)
    // is type-erased in message_generator.
    template<class Body, class Allocator>
    http::message_generator GatewaySession::HandleRequest(http::request<Body, http::basic_fields<Allocator> > &&request) {
        // Make sure we can handle the method
        if (request.method() != http::verb::get && request.method() != http::verb::put &&
            request.method() != http::verb::post && request.method() != http::verb::delete_ &&
            request.method() != http::verb::head) {
            return Core::HttpUtils::BadRequest(request, "Unknown HTTP-method");
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
            auto target = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target");
            handler = GatewayRouter::GetHandler(target, _ioc);
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
            handler = GatewayRouter::GetHandler(authKey.module, _ioc);
            if (!handler) {
                log_error << "Handler not found, target: " << authKey.module;
                return Core::HttpUtils::BadRequest(request, "Handler not found");
            }
            log_trace << "Handler found, name: " << handler->name();
        }

        if (handler) {
            switch (request.method()) {
                case http::verb::get: {
                    Monitoring::MonitoringTimer measure{GATEWAY_HTTP_TIMER, GATEWAY_HTTP_COUNTER, "method", "GET"};
                    return handler->HandleGetRequest(request, _region, _user);
                }
                case http::verb::put: {
                    Monitoring::MonitoringTimer measure{GATEWAY_HTTP_TIMER, GATEWAY_HTTP_COUNTER, "method", "PUT"};
                    return handler->HandlePutRequest(request, _region, _user);
                }
                case http::verb::post: {
                    Monitoring::MonitoringTimer measure{GATEWAY_HTTP_TIMER, GATEWAY_HTTP_COUNTER, "method", "POST"};
                    return handler->HandlePostRequest(request, _region, _user);
                }
                case http::verb::delete_: {
                    Monitoring::MonitoringTimer measure{GATEWAY_HTTP_TIMER, GATEWAY_HTTP_COUNTER, "method", "DELETE"};
                    return handler->HandleDeleteRequest(request, _region, _user);
                }
                case http::verb::head: {
                    Monitoring::MonitoringTimer measure{GATEWAY_HTTP_TIMER, GATEWAY_HTTP_COUNTER, "method", "HEAD"};
                    handler->HandleHeadRequest(request, _region, _user);
                }
                default:
                    return Core::HttpUtils::NotImplemented(request, "Not yet implemented");
            }
        }
        return Core::HttpUtils::BadRequest(request, "Invalid target path");
    }


    void GatewaySession::QueueWrite(http::message_generator response) {
        // Allocate and store the work
        _response_queue.push(std::move(response));

        // If there was no previous work, start the write loop
        if (_response_queue.size() == 1) DoWrite();
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
        if (!keep_alive) {
            return DoClose();
        }

        // Resume the read if it has been paused
        if (_response_queue.size() == _queueLimit)
            DoRead();

        _response_queue.pop();

        DoWrite();
    }

    void GatewaySession::DoClose() {
        // Send a TCP shutdown
        boost::beast::error_code ec;
        _stream.socket().shutdown(ip::tcp::socket::shutdown_send, ec);
        // At this point the connection is closed gracefully
    }

    Core::AuthorizationHeaderKeys GatewaySession::GetAuthorizationKeys(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey) {
        // Get signing version
        Core::AuthorizationHeaderKeys authKeys = {};

        if (const std::string authorizationHeader = request["Authorization"]; !authorizationHeader.empty()) {
            authKeys.signingVersion = Core::StringUtils::Split(authorizationHeader, " ")[0];

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
        response.keep_alive(request.keep_alive());
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

    void GatewaySession::HandleContinueRequest(boost::beast::tcp_stream &_stream, const http::request<http::dynamic_body> &request) {
        http::response<http::empty_body> response;
        response.version(11);
        response.keep_alive(request.keep_alive());
        response.result(http::status::continue_);
        response.set(http::field::server, "awsmock");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::allow, "*/*");
        response.set(http::field::content_length, "0");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "*");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
        response.set(http::field::access_control_max_age, "86400");
        response.set(http::field::keep_alive, "timeout=10, max=100");
        response.set(http::field::connection, "Keep-Alive");

        boost::beast::error_code ec;
        write(_stream, response, ec);
    }
} // namespace AwsMock::Service
