//
// Created by vogje01 on 9/2/25.
//

#include <awsmock/service/apigateway/ApiGatewayService.h>

namespace AwsMock::Service {

    Dto::ApiGateway::CreateApiKeyResponse ApiGatewayService::CreateApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_api_key");
        log_debug << "Create API key request, region:  " << request.region << " customerId: " << request.customerId;

        if (_apiGatewayDatabase.ApiKeyExists(request.region, request.name)) {
            log_error << "API key exists already, region: " << request.region << " name: " << request.customerId;
            throw Core::ServiceException("API key exists already, region: " + request.region + " name: " + request.customerId);
        }

        try {

            // Generate API key
            Database::Entity::ApiGateway::Key key = Dto::ApiGateway::Mapper::map(request);

            // Set enabled
            key.enabled = true;

            // Customer ID
            if (key.id.empty()) {
                key.id = Core::AwsUtils::CreateApiGatewayKeyId();
            }

            // API Key
            if (key.keyValue.empty()) {
                key.keyValue = Core::AwsUtils::CreateApiGatewayApiKey();
            }

            // Save to the database
            key = _apiGatewayDatabase.CreateKey(key);

            log_trace << "Api key created, name: " + key.ToJson();
            return Dto::ApiGateway::Mapper::map(request, key);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::ApiGateway::GetApiKeysResponse ApiGatewayService::GetApiKeys(const Dto::ApiGateway::GetApiKeysRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_api_keys");
        log_debug << "Get API keys request, region:  " << request.region;

        try {

            // Get the list of API keys
            const std::vector<Database::Entity::ApiGateway::Key> keys = _apiGatewayDatabase.GetApiKeys(request.nameQuery, request.customerId, request.position, request.limit);

            log_trace << "Get API keys, count: " << keys.size();
            Dto::ApiGateway::GetApiKeysResponse response{};
            response.region = request.region;
            response.user = request.user;
            response.requestId = request.requestId;
            response.position = keys.empty() ? "" : keys.end()->id;
            response.items = Dto::ApiGateway::Mapper::map(keys);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void ApiGatewayService::DeleteApiKey(const Dto::ApiGateway::DeleteApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_api_key");
        log_debug << "Delete API key request, region:  " << request.region << " apiKey: " << request.apiKey;

        if (!_apiGatewayDatabase.ApiKeyExists(request.apiKey)) {
            log_error << "API key does not exist, region: " << request.region << " apiKey: " << request.apiKey;
            throw Core::ServiceException("API key does not exist, region: " + request.region + " apiKey: " + request.apiKey);
        }

        try {

            // Delete it from the database
            _apiGatewayDatabase.DeleteKey(request.apiKey);

            log_trace << "Api key deleted, id: " + request.apiKey;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Service