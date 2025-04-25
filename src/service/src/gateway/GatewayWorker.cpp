//
// Created by vogje01 on 12/17/24.
//

#include <awsmock/service/gateway/GatewayWorker.h>

namespace AwsMock::Service {

    GatewayWorker::GatewayWorker(tcp::acceptor &acceptor, std::string docRoot) : _acceptor(acceptor), _docRoot(std::move(docRoot)) {

        const Core::Configuration &configuration = Core::Configuration::instance();
        //_queueLimit = configuration.GetValue<int>("awsmock.gateway.http.max-queue");
        //_bodyLimit = configuration.GetValue<int>("awsmock.gateway.http.max-body");
        //_timeout = configuration.GetValue<int>("awsmock.gateway.http.timeout");
        _verifySignature = configuration.GetValue<bool>("awsmock.aws.signature.verify");
        _routingTable = {
                {"s3", std::make_shared<S3Handler>()},
                {"s3api", std::make_shared<S3Handler>()},
                {"sqs", std::make_shared<SQSHandler>()},
                {"sns", std::make_shared<SNSHandler>()},
                {"lambda", std::make_shared<LambdaHandler>()},
                {"transfer", std::make_shared<TransferHandler>()},
                {"cognito-idp", std::make_shared<CognitoHandler>()},
                {"cognito-identity", std::make_shared<CognitoHandler>()},
                {"secretsmanager", std::make_shared<SecretsManagerHandler>()},
                {"kms", std::make_shared<KMSHandler>()},
                {"ssm", std::make_shared<SSMHandler>()},
                {"dynamodb", std::make_shared<DynamoDbHandler>()},
                {"monitoring", std::make_shared<MonitoringHandler>()},
                {"module", std::make_shared<ModuleHandler>()}};
    }

    void
    GatewayWorker::Start() {
        Accept();
        CheckDeadline();
    }

    void GatewayWorker::Accept() {

        int timeout = Core::Configuration::instance().GetValue<int>("awsmock.frontend.timeout");

        // Clean up any previous connection.
        beast::error_code ec = _socket.close(ec);
        if (ec) {
            log_error << "Close socket failed: " << ec.message();
        }
        _buffer.consume(_buffer.size());

        _acceptor.async_accept(_socket, [this, timeout](const beast::error_code &errorCode) {
            if (errorCode) {
                Accept();
            } else {
                // Request must be fully processed within timeout seconds.
                _requestDeadline.expires_after(std::chrono::seconds(timeout));

                ReadRequest();
            }
        });
    }

    void GatewayWorker::ReadRequest() {
        // On each read the parser needs to be destroyed and recreated. We store it in a boost::optional to achieve that.
        //
        // Arguments passed to the parser constructor are forwarded to the message object. A single argument is forwarded
        // to the body constructor.
        //
        // We construct the dynamic body with a 1MB limit to prevent vulnerability to buffer attacks.
        //
        _gatewayParser.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));

        // No limit
        _gatewayParser->body_limit(boost::none);

        async_read(_socket, _buffer, *_gatewayParser, [this](const beast::error_code &ec, std::size_t) {
            if (ec)
                Accept();
            else
                ProcessRequest(_gatewayParser->get());
        });
    }

    void GatewayWorker::ProcessRequest(http::request<request_body_t, http::basic_fields<alloc_t>> const &request) {

        // Make sure we can handle the method
        if (request.method() != http::verb::get && request.method() != http::verb::put &&
            request.method() != http::verb::post && request.method() != http::verb::delete_ &&
            request.method() != http::verb::head && request.method() != http::verb::connect &&
            request.method() != http::verb::options) {
            SendBadResponse(http::status::bad_request, "Invalid request-method");
        }

        // Ping request
        if (request.method() == http::verb::connect) {
            log_debug << "Handle CONNECT request";
            Monitoring::MetricServiceTimer headTimer(GATEWAY_HTTP_TIMER, "method", "CONNECT");
            Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "CONNECT");
            SendOkResponse();
        }

        // Process OPTIONS requests
        if (request.method() == http::verb::options) {
            return SendOptionsResponse();
        }

        std::shared_ptr<AbstractHandler> handler;
        std::string region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        if (Core::HttpUtils::HasHeader(request, "x-awsmock-target")) {

            const std::string target = Core::HttpUtils::GetHeaderValue(request, "x-awsmock-target");
            handler = _routingTable[target];
            log_trace << "Handler found, name: " << handler->name();

        } else {

            // Verify AWS signature
            if (_verifySignature && !Core::AwsUtils::VerifySignature(request, "none")) {
                log_warning << "AWS signature could not be verified";
                //return Core::HttpUtils::Unauthorized(request, "AWS signature could not be verified");
            }

            // Get the module from the authorization key, or the target header field.
            Core::AuthorizationHeaderKeys authKey = GetAuthorizationKeys(request, {});

            region = authKey.region;
            handler = _routingTable[authKey.module];
            log_trace << "Handler found, name: " << handler->name();
        }

        if (handler) {
            switch (request.method()) {
                /*case http::verb::get: {
                    Monitoring::MetricServiceTimer getTimer(GATEWAY_HTTP_TIMER, "method", "GET");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "GET");
                    http::response<http::dynamic_body> response = handler->HandleGetRequest(request, region, "none");
                }
                case http::verb::put: {
                    Monitoring::MetricServiceTimer putTimer(GATEWAY_HTTP_TIMER, "method", "PUT");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "PUT");
                    http::response<http::dynamic_body> response = handler->HandlePutRequest(request, region, "none");
                }
                case http::verb::post: {
                    Monitoring::MetricServiceTimer postTimer(GATEWAY_HTTP_TIMER, "method", "POST");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "POST");
                    http::response<http::dynamic_body> response = handler->HandlePostRequest(request, region, "none");
                }
                case http::verb::delete_: {
                    Monitoring::MetricServiceTimer deleteTimer(GATEWAY_HTTP_TIMER, "method", "DELETE");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "DELETE");
                    http::response<http::dynamic_body> response = handler->HandleDeleteRequest(request, region, "none");
                }
                case http::verb::head: {
                    Monitoring::MetricServiceTimer headTimer(GATEWAY_HTTP_TIMER, "method", "HEAD");
                    Monitoring::MetricService::instance().IncrementCounter(GATEWAY_HTTP_COUNTER, "method", "HEAD");
                    http::response<http::dynamic_body> response = handler->HandleHeadRequest(request, region, "none");
                }*/
                default:
                    SendBadResponse(http::status::bad_request, "Invalid request-method '" + std::string(request.method_string()) + "'\r\n");
                    break;
            }
        }
    }

    void GatewayWorker::SendBadResponse(const http::status status, std::string const &error) {

        _stringResponse.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));
        _stringResponse->result(status);
        _stringResponse->keep_alive(false);
        _stringResponse->set(http::field::server, "Beast");
        _stringResponse->set(http::field::content_type, "text/plain");
        _stringResponse->body() = error;
        _stringResponse->prepare_payload();
        _stringSerializer.emplace(*_stringResponse);

        http::async_write(_socket, *_stringSerializer, [this](beast::error_code ec, std::size_t) {
            ec = _socket.shutdown(tcp::socket::shutdown_send, ec);
            if (ec) {
                log_error << "Shutdown socket failed: " << ec.message();
            }
            _stringSerializer.reset();
            _stringResponse.reset();
            Accept();
        });
    }

    void GatewayWorker::SendFile(const beast::string_view target) {

        // Request path must be absolute and not contain "..".
        if (target.empty() || target[0] != '/' || target.find("..") != std::string::npos) {
            log_error << "File not found, path: " << std::string(target);
            SendBadResponse(http::status::not_found, "File not found\r\n");
            return;
        }
        /*
        std::string full_path = _docRoot;
        if (target.size() == 1) {
            target = boost::string_view(DEFAULT_PAGE.c_str(), DEFAULT_PAGE.length());
        }
        full_path.append(target.data(), target.size());
        log_debug << "Serving: " << full_path;

        http::file_body::value_type file;
        beast::error_code ec;
        file.open(full_path.c_str(), beast::file_mode::read, ec);
        if (ec) {
            target = boost::string_view(DEFAULT_PAGE.c_str(), DEFAULT_PAGE.length());
            full_path = _docRoot + std::string(target);
            file.open(full_path.c_str(), beast::file_mode::read, ec);
            log_trace << std::string(target) << " MIME: " << mime_type(target);
        }

        _fileResponse.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));
        _fileResponse->result(http::status::ok);
        _fileResponse->keep_alive(false);
        _fileResponse->set(http::field::server, "Beast");
        _fileResponse->set(http::field::content_type, mime_type(target));
        _fileResponse->body() = std::move(file);
        _fileResponse->prepare_payload();
        _fileSerializer.emplace(*_fileResponse);
*/
        http::async_write(_socket, *_fileSerializer, [this](beast::error_code errorCode, std::size_t) {
            errorCode = _socket.shutdown(tcp::socket::shutdown_send, errorCode);
            if (errorCode) {
                log_error << "Shutdown socket failed: " << errorCode.message();
            }
            _fileSerializer.reset();
            _fileResponse.reset();
            Accept();
        });
    }

    void GatewayWorker::SendOkResponse() {

        _stringResponse.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));
        _stringResponse->result(http::status::ok);
        _stringResponse->keep_alive(false);
        _stringResponse->set(http::field::server, "Beast");
        _stringResponse->set(http::field::content_type, "text/plain");
        _stringSerializer.emplace(*_stringResponse);

        http::async_write(_socket, *_stringSerializer, [this](beast::error_code ec, std::size_t) {
            ec = _socket.shutdown(tcp::socket::shutdown_send, ec);
            if (ec) {
                log_error << "Shutdown socket failed: " << ec.message();
            }
            _stringSerializer.reset();
            _stringResponse.reset();
            Accept();
        });
    }

    void GatewayWorker::SendOptionsResponse() {

        _stringResponse.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(_alloc));
        _stringResponse->result(http::status::ok);
        _stringResponse->keep_alive(false);
        _stringResponse->set(http::field::server, "Beast");
        _stringResponse->set(http::field::content_type, "text/plain");
        _stringResponse->set(http::field::allow, "*/*");
        _stringResponse->set(http::field::access_control_allow_origin, "*");
        _stringResponse->set(http::field::access_control_allow_headers, "*");
        _stringResponse->set(http::field::access_control_allow_methods, "GET,PUT,POST,DELETE,HEAD,OPTIONS");
        _stringResponse->set(http::field::access_control_max_age, "86400");
        _stringSerializer.emplace(*_stringResponse);

        http::async_write(_socket, *_stringSerializer, [this](beast::error_code ec, std::size_t) {
            ec = _socket.shutdown(tcp::socket::shutdown_send, ec);
            if (ec) {
                log_error << "Shutdown socket failed: " << ec.message();
            }
            _stringSerializer.reset();
            _stringResponse.reset();
            Accept();
        });
    }

    void GatewayWorker::CheckDeadline() {
        // The deadline may have moved, so check it has really passed.
        if (_requestDeadline.expiry() <= std::chrono::steady_clock::now()) {
            // Close socket to cancel any outstanding operation.
            _socket.close();

            // Sleep indefinitely until we're given a new deadline.
            _requestDeadline.expires_at((std::chrono::steady_clock::time_point::max)());
        }

        _requestDeadline.async_wait([this](beast::error_code) {
            CheckDeadline();
        });
    }

    Core::AuthorizationHeaderKeys GatewayWorker::GetAuthorizationKeys(const http::request<request_body_t, http::basic_fields<alloc_t>> &request, const std::string &secretAccessKey) {

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
};// namespace AwsMock::Service
