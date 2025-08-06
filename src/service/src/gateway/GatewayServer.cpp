//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/gateway/GatewayServer.h>

namespace AwsMock::Service {

    GatewayServer::GatewayServer(boost::asio::io_context &ios) : AbstractServer("gateway"), _ios(ios) {


        // Defaults for region and user; will be overwritten by authorized requests
        _region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        _user = Core::Configuration::instance().GetValue<std::string>("awsmock.user");
        _verifySignature = Core::Configuration::instance().GetValue<bool>("awsmock.aws.signature.verify");

        // Get HTTP configuration values
        _port = Core::Configuration::instance().GetValue<int>("awsmock.gateway.http.port");
        _address = Core::Configuration::instance().GetValue<std::string>("awsmock.gateway.http.address");
        log_debug << "Gateway server initialized";
        /*

        // Check module active
        if (!IsActive("gateway")) {
            log_info << "Gateway module inactive";
            return;
        }

        // Set running
        SetRunning();

        // Create and launch a listening port
        const auto listener = std::make_shared<GatewayListener>(_ios, ip::tcp::endpoint{ip::make_address(_address), _port});
        listener->Run();
        log_info << "Gateway server started, endpoint: " << _address << " port: " << _port;
        */

        boost::system::error_code ec;
        auto const endpoint = ip::tcp::endpoint{ip::make_address(_address, ec), _port};
        if (ec) {
            log_error << "Error creating address from string \"" << _address << "\": " << ec.message();
        }

        // Spawn a listening port
        boost::asio::co_spawn(_ios, DoListen(endpoint), [](std::exception_ptr e) {
            if (e)
                try {
                    std::rethrow_exception(e);
                } catch (std::exception &ex) {
                    log_error << "Error in acceptor: " << ex.what();
                }
        });
    }

    boost::asio::awaitable<void> GatewayServer::DoListen(ip::tcp::endpoint endpoint) {

        // Open the acceptor
        auto acceptor = net::use_awaitable.as_default_on(tcp::acceptor(co_await net::this_coro::executor));
        acceptor.open(endpoint.protocol());

        // Allow address reuse
        acceptor.set_option(net::socket_base::reuse_address(true));

        // Bind to the server address
        acceptor.bind(endpoint);

        // Start listening for connections
        acceptor.listen(net::socket_base::max_listen_connections);

        for (;;)
            boost::asio::co_spawn(
                    acceptor.get_executor(),
                    DoSession(tcp_stream_t(co_await acceptor.async_accept())),
                    [](std::exception_ptr e) {
                        if (e)
                            try {
                                std::rethrow_exception(e);
                            } catch (std::exception &e) {
                                std::cerr << "Error in session: " << e.what() << "\n";
                            }
                    });
    }


    boost::asio::awaitable<void> GatewayServer::DoSession(tcp_stream_t stream) {

        // This lambda is used to send messages
        try {
            bool isDone = false;

            // This buffer is required to persist across reads
            beast::flat_buffer buffer;
            for (;;) {

                // Set the timeout.
                stream.expires_after(std::chrono::seconds(_timeout));

                // Read a request
                http::request<http::dynamic_body> req;
                co_await http::async_read(stream, buffer, req);

                // Handle the request
                http::message_generator msg = HandleRequest(std::move(stream), std::move(req), isDone);

                // Determine if we should close the connection
                const bool keep_alive = msg.keep_alive();

                // Send the response
                // if (!isDone) {
                co_await beast::async_write(stream, std::move(msg), boost::asio::use_awaitable);
                //}

                if (!keep_alive) {
                    // This means we should close the connection, usually because
                    // the response indicated the "Connection: close" semantic.
                    break;
                }
            }
        } catch (boost::system::system_error &se) {
            if (se.code() != http::error::end_of_stream)
                throw;
        }

        // Send a TCP shutdown
        beast::error_code ec;
        stream.socket().shutdown(ip::tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully we ignore the error because the client might have
        // dropped the connection already.
    }


    // Return a response for the given request.
    //
    // The concrete type of the response message (which depends on the request) is type-erased in message_generator.
    template<class Body, class Allocator>
    http::message_generator GatewayServer::HandleRequest(tcp_stream_t stream, http::request<Body, http::basic_fields<Allocator>> &&request, bool &alreadyResponded) {

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

            auto target = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target");
            handler = GatewayRouter::GetHandler(target, stream);

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
            handler = GatewayRouter::GetHandler(authKey.module, stream);
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
                    return handler->HandlePostRequest(request, _region, _user, alreadyResponded);
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

    Core::AuthorizationHeaderKeys GatewayServer::GetAuthorizationKeys(const http::request<http::dynamic_body> &request, const std::string &secretAccessKey) {

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

    http::response<http::dynamic_body> GatewayServer::HandleOptionsRequest(const http::request<http::dynamic_body> &request) {

        // Prepare the response message
        http::response<http::dynamic_body> response;
        response.version(request.version());
        response.keep_alive(request.keep_alive());
        response.result(http::status::ok);
        response.set(http::field::server, "awsmock");
        response.set(http::field::date, Core::DateTimeUtils::HttpFormatNow());
        response.set(http::field::allow, "*/*");
        response.set(http::field::access_control_allow_origin, "*");
        response.set(http::field::access_control_allow_headers, "*");
        response.set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
        response.set(http::field::access_control_max_age, "86400");
        response.set(http::field::vary, "Accept-Encoding, Origin");
        response.prepare_payload();

        // Send the response to the client
        return response;
    }

    void GatewayServer::HandleContinueRequest(beast::tcp_stream &_stream) {
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
        beast::error_code ec;
        write(_stream, response, ec);
    }

}// namespace AwsMock::Service
