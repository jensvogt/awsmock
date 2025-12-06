//
// Created by vogje01 on 9/2/25.
//

#include "awsmock/dto/common/mapper/Mapper.h"


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
            Database::Entity::ApiGateway::ApiKey key = Dto::ApiGateway::Mapper::map(request);

            // Set enabled
            key.enabled = true;

            // ID
            if (key.id.empty()) {
                key.id = Core::AwsUtils::CreateApiKeyId();
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
            const std::vector<Database::Entity::ApiGateway::ApiKey> keys = _apiGatewayDatabase.GetApiKeys(request.nameQuery, request.customerId, request.position, request.limit);

            log_trace << "Get API keys, count: " << keys.size();
            Dto::ApiGateway::GetApiKeysResponse response{};
            response.region = request.region;
            response.user = request.user;
            response.requestId = request.requestId;
            response.position = keys.empty() ? "" : keys.size() > 1 ? keys.end()->id
                                                                    : keys.begin()->id;
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

    Dto::ApiGateway::CreateRestApiResponse ApiGatewayService::CreateRestApi(const Dto::ApiGateway::CreateRestApiRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_rest_api");
        log_debug << "Create REST API request, region:  " << request.region << ", name: " << request.name;

        if (_apiGatewayDatabase.ApiKeyExists(request.region, request.name)) {
            log_error << "API key exists already, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("API key exists already, region: " + request.region + " name: " + request.name);
        }

        try {

            // Get rest api entity from request
            Database::Entity::ApiGateway::RestApi restApi = Dto::ApiGateway::Mapper::map(request);

            // ID
            if (restApi.id.empty()) {
                restApi.id = Core::AwsUtils::CreateRestApiId();
            }

            // Save to the database
            restApi = _apiGatewayDatabase.CreateRestApi(restApi);

            log_trace << "REST API created, name: " + restApi.name;
            return Dto::ApiGateway::Mapper::map(request, restApi);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::ApiGateway::ListApiKeyCountersResponse ApiGatewayService::ListApiKeyCounters(const Dto::ApiGateway::ListApiKeyCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_api_keys");
        log_debug << "List API key counters request, region:  " << request.region;

        try {

            // Get the list of API keys
            const std::vector<Database::Entity::ApiGateway::ApiKey> keys = _apiGatewayDatabase.ListApiKeyCounters(request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            const long total = _apiGatewayDatabase.CountApiKeys();

            log_trace << "Get API keys, count: " << keys.size();
            Dto::ApiGateway::ListApiKeyCountersResponse response{};
            response.total = total;
            response.apiKeys = Dto::ApiGateway::Mapper::map(keys);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::ApiGateway::GetApiKeyCounterResponse ApiGatewayService::GetApiKeyCounter(const Dto::ApiGateway::GetApiKeyCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_api_key");
        log_debug << "Get API key counter request, region:  " << request.region;

        if (!_apiGatewayDatabase.ApiKeyExists(request.id)) {
            log_error << "API key does not exist, region: " << request.region << ", id: " << request.id;
            throw Core::ServiceException("API key does not exist, region: " + request.region + ", apiKey: " + request.id);
        }

        try {

            // Get the API key
            Database::Entity::ApiGateway::ApiKey key = _apiGatewayDatabase.GetApiKeyById(request.id);

            Dto::ApiGateway::GetApiKeyCounterResponse response{};
            response.apiKey = Dto::ApiGateway::Mapper::map(key);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void ApiGatewayService::UpdateApiKeyCounter(const Dto::ApiGateway::UpdateApiKeyCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "update_api_key");
        log_debug << "Update API key counter request, region:  " << request.apiKey.id;

        if (!_apiGatewayDatabase.ApiKeyExists(request.apiKey.id)) {
            log_error << "API key does not exist, region: " << request.region << " id: " << request.apiKey.id;
            throw Core::ServiceException("API key does not exist, region: " + request.region + " id: " + request.apiKey.id);
        }

        try {

            // Update the API key
            Database::Entity::ApiGateway::ApiKey key = Dto::ApiGateway::Mapper::map(request.apiKey);
            key = _apiGatewayDatabase.UpdateApiKey(key);
            log_debug << "Api key updated, id: " + key.id;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::ApiGateway::ListRestApiCountersResponse ApiGatewayService::ListRestApiCounters(const Dto::ApiGateway::ListRestApiCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "list_rest_apis");
        log_debug << "List REST API counters request, region:  " << request.region;

        try {

            // Get the list of REST APIs
            const std::vector<Database::Entity::ApiGateway::RestApi> restApis = _apiGatewayDatabase.ListRestApiCounters(request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            const long total = _apiGatewayDatabase.CountApiKeys();

            log_trace << "Get REST APIs, count: " << restApis.size();
            Dto::ApiGateway::ListRestApiCountersResponse response{};
            response.total = total;
            response.restApis = Dto::ApiGateway::Mapper::map(restApis);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Service