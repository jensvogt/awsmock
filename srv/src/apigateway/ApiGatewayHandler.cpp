//
// Created by vogje01 on 9/2/25.
//

#include <awsmock/service/apigateway/ApiGatewayHandler.h>

namespace Awsmock::Service {

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

                default:
                    log_error << "Unknown action";
                    return SendResponse(request, http::status::bad_request, "Unknown action");
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

                default:
                    log_error << "Unknown action";
                    return SendResponse(request, http::status::bad_request, "Unknown action");
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

                default:
                    log_error << "Unknown action";
                    return SendResponse(request, http::status::bad_request, "Unknown action");
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

}// namespace Awsmock::Service