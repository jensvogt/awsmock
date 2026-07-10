//
// Created by vogje01 on 9/2/25.
//

// C++ includes
#include <sstream>

// Awsmock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/apigateway/DeleteResourceRequest.h>
#include <awsmock/dto/apigateway/DeleteUsagePlanRequest.h>
#include <awsmock/dto/apigateway/GetResourceRequest.h>
#include <awsmock/service/apigateway/ApiGatewayHandler.h>

namespace Awsmock::Service {

    namespace {

        // Parse the Lambda function name out of an authorizer URI.
        // authorizerUri = "arn:aws:apigateway:...:lambda:path/2015-03-31/functions/arn:aws:lambda:...:function:name/invocations"
        std::string ExtractFunctionNameFromUri(const std::string &authorizerUri) {
            constexpr std::string_view marker = "/functions/";
            const auto pos = authorizerUri.find(marker);
            if (pos == std::string::npos) {
                return {};
            }
            std::string lambdaArn = authorizerUri.substr(pos + marker.size());
            if (const auto invPos = lambdaArn.find("/invocations"); invPos != std::string::npos) {
                lambdaArn = lambdaArn.substr(0, invPos);
            }
            // Function name is the last colon-separated component of the Lambda ARN
            if (const auto fnPos = lambdaArn.rfind(':'); fnPos != std::string::npos) {
                return lambdaArn.substr(fnPos + 1);
            }
            return lambdaArn;
        }

        // Build the minimal JSON event sent to a Lambda authorizer.
        std::string BuildAuthorizerPayload(const http::request<http::dynamic_body> &request, const std::string &restApiId, const std::string &resourcePath, const Database::Entity::ApiGateway::Authorizer &authorizer) {
            const auto httpMethod = boost::lexical_cast<std::string>(request.method());

            // Build the request context
            document requestContext;
            Core::Bson::BsonUtils::SetStringValue(requestContext, "apiId", restApiId);
            Core::Bson::BsonUtils::SetStringValue(requestContext, "resourcePath", resourcePath);
            Core::Bson::BsonUtils::SetStringValue(requestContext, "httpMethod", httpMethod);
            Core::Bson::BsonUtils::SetStringValue(requestContext, "stage", "test");
            Core::Bson::BsonUtils::SetStringValue(requestContext, "requestId", Core::StringUtils::GenerateRandomHexString(16));

            // Build the event
            document event;
            Core::Bson::BsonUtils::SetStringValue(event, "type", authorizer.type.empty() ? "REQUEST" : authorizer.type);
            Core::Bson::BsonUtils::SetStringValue(event, "resource", resourcePath);
            Core::Bson::BsonUtils::SetStringValue(event, "path", resourcePath);
            Core::Bson::BsonUtils::SetStringValue(event, "httpMethod", httpMethod);
            Core::Bson::BsonUtils::SetStringValue(event, "methodArn", "arn:aws:execute-api::" + restApiId + "/test/" + httpMethod + resourcePath);
            Core::Bson::BsonUtils::SetDocumentValue(event, "requestContext", requestContext);
            return Core::Bson::BsonUtils::ToJsonString(event.view());
        }

        // Returns true if the authorizer Lambda response contains an Allow statement.
        bool IsInvocationAllowed(const std::string &responseBody) {
            return responseBody.find("\"Allow\"") != std::string::npos;
        }

        // Build an API Gateway Lambda proxy event (version 1.0).
        std::string BuildLambdaProxyEvent(const http::request<http::dynamic_body> &request, const std::string &restApiId, const std::string &resourcePath, const std::string &rawUrl) {
            const auto httpMethod = boost::lexical_cast<std::string>(request.method());

            // Headers
            document headersDoc;
            for (const auto &field: request.base()) {
                Core::Bson::BsonUtils::SetStringValue(headersDoc, std::string(field.name_string()), std::string(field.value()));
            }

            // Query string parameters
            document queryDoc;
            for (const auto &[name, value]: Core::HttpUtils::GetQueryParameters(rawUrl)) {
                Core::Bson::BsonUtils::SetStringValue(queryDoc, name, value);
            }

            // Build the requestContext
            document requestContext;
            Core::Bson::BsonUtils::SetStringValue(requestContext, "apiId", restApiId);
            Core::Bson::BsonUtils::SetStringValue(requestContext, "resourcePath", resourcePath);
            Core::Bson::BsonUtils::SetStringValue(requestContext, "httpMethod", httpMethod);
            Core::Bson::BsonUtils::SetStringValue(requestContext, "stage", "local");
            Core::Bson::BsonUtils::SetStringValue(requestContext, "requestId", Core::StringUtils::GenerateRandomHexString(16));

            // Build the event
            document event;
            Core::Bson::BsonUtils::SetStringValue(event, "version", "1.0");
            Core::Bson::BsonUtils::SetStringValue(event, "resource", resourcePath);
            Core::Bson::BsonUtils::SetStringValue(event, "path", resourcePath);
            Core::Bson::BsonUtils::SetStringValue(event, "httpMethod", httpMethod);
            Core::Bson::BsonUtils::SetDocumentValue(event, "headers", headersDoc);
            Core::Bson::BsonUtils::SetDocumentValue(event, "queryStringParameters", queryDoc);
            Core::Bson::BsonUtils::SetDocumentValue(event, "requestContext", requestContext);
            Core::Bson::BsonUtils::SetStringValue(event, "body", Core::HttpUtils::GetBodyAsString(request));
            Core::Bson::BsonUtils::SetBoolValue(event, "isBase64Encoded", false);
            return Core::Bson::BsonUtils::ToJsonString(event.view());
        }

        // Parse a Lambda proxy response and return the corresponding HTTP response.
        // Expected Lambda output: {"statusCode": 200, "headers": {...}, "body": "..."}
        http::response<http::dynamic_body> BuildLambdaProxyResponse(const http::request<http::dynamic_body> &request, const int lambdaStatus, const std::string &responseBody) {
            int statusCode = lambdaStatus;
            std::string body;
            std::map<std::string, std::string> headers;

            if (!responseBody.empty()) {
                try {
                    const auto parsed = Core::Json::ParseJsonString(responseBody);
                    if (Core::Json::AttributeExists(parsed, "statusCode")) {
                        statusCode = Core::Json::GetIntValue(parsed, "statusCode");
                    }
                    body = Core::Json::GetStringValue(parsed, "body");
                    if (Core::Json::AttributeExists(parsed, "headers") && parsed.at("headers").is_object()) {
                        for (const auto &[k, v]: parsed.at("headers").as_object()) {
                            headers[k] = v.as_string().data();
                        }
                    }
                } catch (...) {
                    body = responseBody;
                }
            }

            http::response<http::dynamic_body> response{static_cast<http::status>(statusCode), request.version()};
            response.set(http::field::server, "AwsMock");
            response.set(http::field::content_type, "application/json");
            for (const auto &[k, v]: headers) {
                response.set(k, v);
            }
            boost::beast::ostream(response.body()) << body;
            response.prepare_payload();
            return response;
        }

        // Match a resource path pattern (which may contain {param} segments) against a concrete request path.
        bool PathMatches(const std::string &pattern, const std::string &path) {
            if (pattern == path) return true;

            const auto split = [](const std::string &s) {
                std::vector<std::string> parts;
                std::istringstream ss(s);
                std::string tok;
                while (std::getline(ss, tok, '/')) {
                    if (!tok.empty()) parts.push_back(tok);
                }
                return parts;
            };

            const auto pp = split(pattern);
            const auto rp = split(path);
            if (pp.size() != rp.size()) return false;
            for (size_t i = 0; i < pp.size(); ++i) {
                if (pp[i].front() == '{' && pp[i].back() == '}') continue;
                if (pp[i] != rp[i]) return false;
            }
            return true;
        }

        // Forward an HTTP request synchronously to an external URI (HTTP/HTTP_PROXY integration).
        http::response<http::dynamic_body> ForwardHttpRequest(const http::request<http::dynamic_body> &inbound, const std::string &integrationUri, const std::string &integrationHttpMethod) {
            namespace beast = boost::beast;

            boost::system::result<boost::urls::url_view> parsed = boost::urls::parse_uri(integrationUri);
            if (!parsed) {
                http::response<http::dynamic_body> err{http::status::bad_gateway, inbound.version()};
                beast::ostream(err.body()) << "Invalid integration URI";
                err.prepare_payload();
                return err;
            }
            const std::string host = parsed->host();
            const std::string port = parsed->port().empty() ? (parsed->scheme() == "https" ? "443" : "80") : std::string(parsed->port());
            const std::string target = parsed->path().empty() ? "/" : std::string(parsed->path()) + (parsed->query().empty() ? "" : "?" + std::string(parsed->query()));

            try {
                boost::asio::io_context ioc;
                ip::tcp::resolver resolver(ioc);
                beast::tcp_stream stream(ioc);
                stream.connect(Core::HttpUtils::ResolveHost(resolver, host, port));

                const http::verb verb = integrationHttpMethod.empty() ? inbound.method() : http::string_to_verb(integrationHttpMethod);
                http::request<http::string_body> req{verb, target, 11};
                req.set(http::field::host, host);
                req.set(http::field::content_type, "application/json");
                req.body() = Core::HttpUtils::GetBodyAsString(inbound);
                req.prepare_payload();

                http::write(stream, req);

                beast::flat_buffer buf;
                http::response<http::dynamic_body> res;
                http::read(stream, buf, res);
                beast::error_code ec;
                std::ignore = stream.socket().shutdown(ip::tcp::socket::shutdown_both, ec);
                return res;

            } catch (const std::exception &exc) {
                http::response<http::dynamic_body> err{http::status::bad_gateway, inbound.version()};
                beast::ostream(err.body()) << exc.what();
                err.prepare_payload();
                return err;
            }
        }

    }// namespace

    http::response<http::dynamic_body> ApiGatewayHandler::HandleGetRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Application GET request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ApiGatewayClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::ApiGatewayCommandType::GET_API_KEYS: {

                    Dto::ApiGateway::GetApiKeysRequest serviceRequest;
                    serviceRequest.nameQuery = Core::HttpUtils::GetStringParameterFromBody(request.target(), "nameQuery");
                    serviceRequest.customerId = Core::HttpUtils::GetStringParameterFromBody(request.target(), "customerId");
                    serviceRequest.includeValues = Core::HttpUtils::GetBoolParameter(request.target(), "includeValues");
                    serviceRequest.limit = Core::HttpUtils::GetLongParameter(request.target(), "limit", 0, 100, 100);
                    serviceRequest.position = Core::HttpUtils::GetStringParameterFromBody(request.target(), "position");
                    const Dto::ApiGateway::GetApiKeysResponse serviceResponse = _apiGatewayService.getApiKeys(serviceRequest);
                    log_info << "API key list created";
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::GET_REST_APIS: {

                    Dto::ApiGateway::GetRestApisRequest serviceRequest;
                    serviceRequest.limit = Core::HttpUtils::GetLongParameter(request.target(), "limit");
                    serviceRequest.position = Core::HttpUtils::GetLongParameter(request.target(), "position");
                    const Dto::ApiGateway::GetRestApisResponse serviceResponse = _apiGatewayService.getRestApis(serviceRequest);
                    log_info << "REST APIs list created";
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::GET_INTEGRATION: {
                    const std::string restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    const std::string resourceId = Core::HttpUtils::GetPathParameter(request.target(), 3);
                    const std::string httpMethod = Core::HttpUtils::GetPathParameter(request.target(), 5);
                    const auto method = _apiGatewayService.getIntegration(restApiId, resourceId, httpMethod);
                    log_info << "Get integration, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;
                    return SendResponse(request, http::status::ok, method.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::GET_RESOURCE: {

                    Dto::ApiGateway::GetResourcesRequest serviceRequest = Dto::ApiGateway::GetResourcesRequest::FromJson(clientCommand);
                    serviceRequest.restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    const Dto::ApiGateway::GetResourcesResponse serviceResponse = _apiGatewayService.getResources(serviceRequest);
                    log_info << "Get REST resources, restApiId: " << serviceRequest.restApiId;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::GET_RESOURCES: {

                    Dto::ApiGateway::GetResourcesRequest serviceRequest = Dto::ApiGateway::GetResourcesRequest::FromJson(clientCommand);
                    serviceRequest.restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    const Dto::ApiGateway::GetResourcesResponse serviceResponse = _apiGatewayService.getResources(serviceRequest);
                    log_info << "Get REST resources, restApiId: " << serviceRequest.restApiId;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                default:
                    return HandleResourceInvocation(request, region, user);
            }
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, "Unknown action");
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

    http::response<http::dynamic_body> ApiGatewayHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Application POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ApiGatewayClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::ApiGatewayCommandType::CREATE_API_KEY: {

                    Dto::ApiGateway::CreateApiKeyRequest serviceRequest = Dto::ApiGateway::CreateApiKeyRequest::FromJson(clientCommand);
                    const Dto::ApiGateway::CreateApiKeyResponse serviceResponse = _apiGatewayService.createApiKey(serviceRequest);
                    log_info << "API key created, name: " << serviceRequest.name;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::DELETE_API_KEY: {

                    Dto::ApiGateway::CreateApiKeyRequest serviceRequest = Dto::ApiGateway::CreateApiKeyRequest::FromJson(clientCommand);
                    const Dto::ApiGateway::CreateApiKeyResponse serviceResponse = _apiGatewayService.createApiKey(serviceRequest);
                    log_info << "API key created, name: " << serviceRequest.name;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::LIST_API_KEY_COUNTERS: {

                    Dto::ApiGateway::ListApiKeyCountersRequest serviceRequest = Dto::ApiGateway::ListApiKeyCountersRequest::FromJson(clientCommand);
                    const Dto::ApiGateway::ListApiKeyCountersResponse serviceResponse = _apiGatewayService.listApiKeyCounters(serviceRequest);
                    log_info << "API key counter list created, count: " << serviceResponse.total;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::GET_API_KEY_COUNTER: {

                    Dto::ApiGateway::GetApiKeyCounterRequest serviceRequest = Dto::ApiGateway::GetApiKeyCounterRequest::FromJson(clientCommand);
                    const Dto::ApiGateway::GetApiKeyCounterResponse serviceResponse = _apiGatewayService.getApiKeyCounter(serviceRequest);
                    log_info << "Get API key counter, id: " << serviceRequest.id;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::UPDATE_API_KEY_COUNTER: {

                    Dto::ApiGateway::UpdateApiKeyCounterRequest serviceRequest = Dto::ApiGateway::UpdateApiKeyCounterRequest::FromJson(clientCommand);
                    _apiGatewayService.updateApiKeyCounter(serviceRequest);
                    log_info << "Update API key counter, id: " << serviceRequest.apiKey.id;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApiGatewayCommandType::CREATE_REST_API: {

                    Dto::ApiGateway::CreateRestApiRequest serviceRequest = Dto::ApiGateway::CreateRestApiRequest::FromJson(clientCommand);
                    const Dto::ApiGateway::CreateRestApiResponse serviceResponse = _apiGatewayService.createRestApi(serviceRequest);
                    log_info << "REST API created, name: " << serviceRequest.name;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::LIST_REST_API_COUNTERS: {

                    Dto::ApiGateway::ListRestApiCountersRequest serviceRequest = Dto::ApiGateway::ListRestApiCountersRequest::FromJson(clientCommand);
                    Dto::ApiGateway::ListRestApiCountersResponse serviceResponse = _apiGatewayService.listRestApiCounters(serviceRequest);
                    log_info << "List REST API counter, count: " << serviceResponse.total;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::GET_REST_API_COUNTER: {

                    Dto::ApiGateway::GetRestApiCounterRequest serviceRequest = Dto::ApiGateway::GetRestApiCounterRequest::FromJson(clientCommand);
                    Dto::ApiGateway::GetRestApiCounterResponse serviceResponse = _apiGatewayService.getRestApiCounter(serviceRequest);
                    log_info << "Get REST API counter, name: " << serviceRequest.name;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::UPDATE_REST_API_COUNTER: {

                    Dto::ApiGateway::UpdateRestApiCounterRequest serviceRequest = Dto::ApiGateway::UpdateRestApiCounterRequest::FromJson(clientCommand);
                    _apiGatewayService.updateRestApiCounter(serviceRequest);
                    log_info << "Update REST API counter, name: " << serviceRequest.restApi.name;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApiGatewayCommandType::DELETE_REST_API_COUNTER: {

                    Dto::ApiGateway::DeleteRestApiCounterRequest serviceRequest = Dto::ApiGateway::DeleteRestApiCounterRequest::FromJson(clientCommand);
                    _apiGatewayService.deleteRestApiCounter(serviceRequest);
                    log_info << "Delete REST API counter, name: " << serviceRequest.name;
                    return SendResponse(request, http::status::ok);
                }

                case Dto::Common::ApiGatewayCommandType::CREATE_RESOURCE: {

                    Dto::ApiGateway::CreateResourceRequest serviceRequest = Dto::ApiGateway::CreateResourceRequest::FromJson(clientCommand);
                    serviceRequest.restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    serviceRequest.parentId = Core::HttpUtils::GetPathParameter(request.target(), 3);
                    const Dto::ApiGateway::CreateResourceResponse serviceResponse = _apiGatewayService.createResource(serviceRequest);
                    log_info << "REST resource created, pathPart: " << serviceRequest.pathPart;
                    return SendResponse(request, http::status::ok, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::CREATE_USAGE_PLAN: {

                    Dto::ApiGateway::CreateUsagePlanRequest serviceRequest = Dto::ApiGateway::CreateUsagePlanRequest::FromJson(clientCommand);
                    const Dto::ApiGateway::CreateUsagePlanResponse serviceResponse = _apiGatewayService.createUsagePlan(serviceRequest);
                    log_info << "Usage plan created, id: " << serviceResponse.id;
                    return SendResponse(request, http::status::created, serviceResponse.ToJson());
                }

                case Dto::Common::ApiGatewayCommandType::CREATE_USAGE_PLAN_KEY: {

                    Dto::ApiGateway::CreateUsagePlanKeyRequest serviceRequest = Dto::ApiGateway::CreateUsagePlanKeyRequest::FromJson(clientCommand);
                    serviceRequest.usagePlanId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    const Dto::ApiGateway::CreateUsagePlanKeyResponse serviceResponse = _apiGatewayService.createUsagePlanKey(serviceRequest);
                    log_info << "Usage plan key created, usagePlanId: " << serviceRequest.usagePlanId << " keyId: " << serviceRequest.keyId;
                    return SendResponse(request, http::status::created, serviceResponse.ToJson());
                }

                default:
                    return HandleResourceInvocation(request, region, user);
            }
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, "Unknown action");
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

    http::response<http::dynamic_body> ApiGatewayHandler::HandlePutRequest(http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "API Gateway PUT request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ApiGatewayClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::ApiGatewayCommandType::PUT_METHOD: {
                    const std::string restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    const std::string resourceId = Core::HttpUtils::GetPathParameter(request.target(), 3);
                    const std::string httpMethod = Core::HttpUtils::GetPathParameter(request.target(), 5);
                    bool apiKeyRequired = false;
                    if (!clientCommand.payload.empty()) {
                        const auto body = Core::Json::ParseJsonString(clientCommand.payload);
                        apiKeyRequired = Core::Json::GetBoolValue(body, "apiKeyRequired");
                    }
                    _apiGatewayService.putMethod(restApiId, resourceId, httpMethod, apiKeyRequired);
                    log_info << "Method put, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;
                    return SendResponse(request, http::status::ok, clientCommand.payload);
                }

                case Dto::Common::ApiGatewayCommandType::PUT_INTEGRATION: {
                    const std::string restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    const std::string resourceId = Core::HttpUtils::GetPathParameter(request.target(), 3);
                    const std::string httpMethod = Core::HttpUtils::GetPathParameter(request.target(), 5);
                    const auto body = Core::Json::ParseJsonString(clientCommand.payload);
                    const std::string type = Core::Json::GetStringValue(body, "type");
                    const std::string uri = Core::Json::GetStringValue(body, "uri");
                    const std::string integrationHttpMethod = Core::Json::GetStringValue(body, "httpMethod");
                    _apiGatewayService.putIntegration(restApiId, resourceId, httpMethod, type, uri, integrationHttpMethod);
                    log_info << "Integration put, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod << ", type: " << type;
                    return SendResponse(request, http::status::ok, clientCommand.payload);
                }

                default:
                    return HandleResourceInvocation(request, region, user);
            }

        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::bad_request, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::not_found, exc.what());
        }
    }

    http::response<http::dynamic_body> ApiGatewayHandler::HandleDeleteRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_debug << "Application DELETE request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::ApiGatewayClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {
            switch (clientCommand.command) {

                case Dto::Common::ApiGatewayCommandType::DELETE_API_KEY: {

                    Dto::ApiGateway::DeleteApiKeyRequest serviceRequest;
                    serviceRequest.apiKey = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    _apiGatewayService.deleteApiKey(serviceRequest);
                    log_info << "API key deleted, id: " << serviceRequest.apiKey;
                    return SendResponse(request, http::status::accepted);
                }

                case Dto::Common::ApiGatewayCommandType::DELETE_REST_API: {

                    Dto::ApiGateway::DeleteRestApiRequest serviceRequest;
                    serviceRequest.restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    _apiGatewayService.deleteRestApi(serviceRequest);
                    log_info << "REST API deleted, restApiId: " << serviceRequest.restApiId;
                    return SendResponse(request, http::status::accepted);
                }

                case Dto::Common::ApiGatewayCommandType::DELETE_RESOURCE: {

                    Dto::ApiGateway::DeleteResourceRequest serviceRequest;
                    serviceRequest.restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    serviceRequest.resourceId = Core::HttpUtils::GetPathParameter(request.target(), 3);
                    _apiGatewayService.deleteResource(serviceRequest);
                    log_info << "REST API deleted, restApiId: " << serviceRequest.restApiId;
                    return SendResponse(request, http::status::accepted);
                }

                case Dto::Common::ApiGatewayCommandType::DELETE_INTEGRATION: {
                    const std::string restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    const std::string resourceId = Core::HttpUtils::GetPathParameter(request.target(), 3);
                    const std::string httpMethod = Core::HttpUtils::GetPathParameter(request.target(), 5);
                    _apiGatewayService.deleteIntegration(restApiId, resourceId, httpMethod);
                    log_info << "Integration deleted, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;
                    return SendResponse(request, http::status::no_content);
                }

                case Dto::Common::ApiGatewayCommandType::DELETE_USAGE_PLAN: {

                    Dto::ApiGateway::DeleteUsagePlanRequest serviceRequest;
                    serviceRequest.usagePlanId = Core::HttpUtils::GetPathParameter(request.target(), 1);
                    _apiGatewayService.deleteUsagePlan(serviceRequest);
                    log_info << "Usage plan deleted, id: " << serviceRequest.usagePlanId;
                    return SendResponse(request, http::status::accepted);
                }

                default:
                    return HandleResourceInvocation(request, region, user);
            }

        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, "Unknown action");
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendResponse(request, http::status::internal_server_error, exc.message());
        } catch (Core::BadRequestException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        } catch (Core::NotFoundException &exc) {
            log_error << exc.what();
            return SendResponse(request, http::status::internal_server_error, exc.what());
        }
    }

    http::response<http::dynamic_body> ApiGatewayHandler::HandleResourceInvocation(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) const {
        log_debug << "API Gateway resource invocation, URI: " << request.target() << " region: " << region;

        // URL: /restapis/{restApiId}/{resource-path...}
        const std::string restApiId = Core::HttpUtils::GetPathParameter(request.target(), 1);
        if (restApiId.empty()) {
            return SendResponse(request, http::status::not_found, "REST API ID missing");
        }

        // Compute resource path: strip "/restapis/{restApiId}" prefix, drop query string
        const auto url = std::string(request.target());
        const auto prefix = "/restapis/" + restApiId;
        std::string resourcePath = url.starts_with(prefix) ? url.substr(prefix.size()) : "/";
        if (const auto qPos = resourcePath.find('?'); qPos != std::string::npos) {
            resourcePath = resourcePath.substr(0, qPos);
        }
        if (resourcePath.empty()) {
            resourcePath = "/";
        }

        // Look up the REST API
        Database::Entity::ApiGateway::RestApi restApi;
        try {
            restApi = _apiGatewayService.getRestApiById(restApiId);
        } catch (...) {
            log_warning << "REST API not found, restApiId: " << restApiId;
            return SendResponse(request, http::status::not_found, "REST API not found");
        }

        // Find the resource matching the path (supports {param} segments)
        const auto resourceIt = std::ranges::find_if(restApi.resources, [&resourcePath](const auto &pair) {
            return PathMatches(pair.second.path, resourcePath);
        });
        if (resourceIt == restApi.resources.end()) {
            log_warning << "Resource not found, path: " << resourcePath;
            return SendResponse(request, http::status::not_found, "Resource not found");
        }

        const auto &resource = resourceIt->second;
        const auto httpMethod = boost::lexical_cast<std::string>(request.method());

        // Validate API key if the matching method requires it
        if (const auto methodIt = resource.resourceMethods.find(httpMethod); methodIt != resource.resourceMethods.end()) {
            if (methodIt->second.apiKeyRequired) {
                if (const std::string apiKey = Core::HttpUtils::GetHeaderValue(request, "x-api-key"); apiKey.empty() || !_apiGatewayService.validateApiKey(apiKey)) {
                    log_warning << "API key missing or invalid, path: " << resourcePath;
                    return SendResponse(request, http::status::forbidden, "Forbidden");
                }
            }
        }

        // Invoke the Lambda authorizer if one is configured on this REST API
        if (!restApi.authorizers.empty()) {
            const auto &[authorizerId, authorizer] = *restApi.authorizers.begin();
            if (const std::string functionName = ExtractFunctionNameFromUri(authorizer.authorizerUri); !functionName.empty()) {
                std::string payload = BuildAuthorizerPayload(request, restApiId, resourcePath, authorizer);
                try {
                    if (const Dto::Lambda::LambdaResult result = _lambdaService.InvokeLambdaFunction(region, functionName, payload, Dto::Lambda::REQUEST_RESPONSE); !IsInvocationAllowed(result.responseBody)) {
                        log_warning << "Authorization denied by authorizer: " << authorizer.name;
                        return SendResponse(request, http::status::forbidden, "Forbidden");
                    }
                } catch (const std::exception &exc) {
                    log_error << "Authorizer invocation failed: " << exc.what();
                    return SendResponse(request, http::status::internal_server_error, "Authorizer error");
                }
            }
        }

        // Dispatch to the configured integration; fall back to ANY method if exact verb not found
        auto methodIt = resource.resourceMethods.find(httpMethod);
        if (methodIt == resource.resourceMethods.end()) {
            methodIt = resource.resourceMethods.find("ANY");
        }
        if (methodIt == resource.resourceMethods.end()) {
            log_warning << "No method configured for HTTP verb: " << httpMethod << ", path: " << resourcePath;
            return SendResponse(request, http::status::method_not_allowed, "Method not allowed");
        }
        const auto &method = methodIt->second;

        if (method.integrationType == "AWS_PROXY" || method.integrationType == "AWS") {
            const std::string functionName = ExtractFunctionNameFromUri(method.integrationUri);
            if (functionName.empty()) {
                log_warning << "Cannot extract function name from integration URI: " << method.integrationUri;
                return SendResponse(request, http::status::bad_gateway, "Invalid Lambda integration URI");
            }
            std::string payload = BuildLambdaProxyEvent(request, restApiId, resourcePath, url);
            log_info << "Invoking Lambda integration, function: " << functionName << ", restApiId: " << restApiId;
            const Dto::Lambda::LambdaResult result = _lambdaService.InvokeLambdaFunction(region, functionName, payload, Dto::Lambda::REQUEST_RESPONSE);
            return BuildLambdaProxyResponse(request, result.status, result.responseBody);
        }
        if (method.integrationType == "MOCK") {
            log_info << "MOCK integration, restApiId: " << restApiId << ", path: " << resourcePath;
            return SendResponse(request, http::status::ok, "{}");
        }
        if (method.integrationType == "HTTP" || method.integrationType == "HTTP_PROXY") {
            log_info << "HTTP integration, uri: " << method.integrationUri << ", restApiId: " << restApiId;
            return ForwardHttpRequest(request, method.integrationUri, method.integrationHttpMethod);
        }
        if (method.integrationType.empty()) {
            log_warning << "No integration configured, restApiId: " << restApiId << ", path: " << resourcePath;
            return SendResponse(request, http::status::not_implemented, "No integration configured for this method");
        }
        log_warning << "Unsupported integration type: " << method.integrationType;
        return SendResponse(request, http::status::not_implemented, "Integration type not supported: " + method.integrationType);
    }

}// namespace Awsmock::Service