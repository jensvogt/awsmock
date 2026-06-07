//
// Created by vogje01 on 9/2/25.
//

#include "awsmock/dto/common/mapper/SortColumnMapper.h"


#include <awsmock/service/apigateway/ApiGatewayService.h>

namespace Awsmock::Service {

    Dto::ApiGateway::CreateApiKeyResponse ApiGatewayService::createApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_api_key");
        log_debug << "Create API key request, region:  " << request.region << " customerId: " << request.customerId;

        if (_apiGatewayDatabase->apiKeyExists(request.region, request.name)) {
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
            key = _apiGatewayDatabase->createKey(key);

            log_trace << "Api key created, name: " + key.ToJson();
            return Dto::ApiGateway::Mapper::map(request, key);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::GetApiKeysResponse ApiGatewayService::getApiKeys(const Dto::ApiGateway::GetApiKeysRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_api_keys");
        log_debug << "Get API keys request, region:  " << request.region;

        try {

            // Get the list of API keys
            const std::vector<Database::Entity::ApiGateway::ApiKey> keys = _apiGatewayDatabase->listApiKeys(request.nameQuery, request.customerId, request.position, request.limit);

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
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::deleteApiKey(const Dto::ApiGateway::DeleteApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_api_key");
        log_debug << "Delete API key request, region:  " << request.region << " apiKey: " << request.apiKey;

        if (!_apiGatewayDatabase->apiKeyExists(request.apiKey)) {
            log_error << "API key does not exist, region: " << request.region << " apiKey: " << request.apiKey;
            throw Core::ServiceException("API key does not exist, region: " + request.region + " apiKey: " + request.apiKey);
        }

        try {

            // Delete it from the database
            _apiGatewayDatabase->deleteKey(request.apiKey);

            log_trace << "Api key deleted, id: " + request.apiKey;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::CreateRestApiResponse ApiGatewayService::createRestApi(const Dto::ApiGateway::CreateRestApiRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_rest_api");
        log_debug << "Create REST API request, region:  " << request.region << ", name: " << request.name;

        if (_apiGatewayDatabase->apiKeyExists(request.region, request.name)) {
            log_error << "REST API exists already, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("REST API exists already, region: " + request.region + " name: " + request.name);
        }

        try {

            // Get rest api entity from request
            Database::Entity::ApiGateway::RestApi restApi = Dto::ApiGateway::Mapper::map(request);

            // ID
            if (restApi.id.empty()) {
                restApi.id = Core::AwsUtils::CreateRestApiId();
            }

            // Save to the database
            restApi = _apiGatewayDatabase->createRestApi(restApi);

            log_trace << "REST API created, name: " + restApi.name;
            return Dto::ApiGateway::Mapper::map(request, restApi);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::GetRestApisResponse ApiGatewayService::getRestApis(const Dto::ApiGateway::GetRestApisRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_rest_apis");
        log_debug << "Get REST APIs request, region:  " << request.region;

        try {

            // Get the list of API keys
            const std::vector<Database::Entity::ApiGateway::RestApi> restApis = _apiGatewayDatabase->listRestApis(request.region);

            log_trace << "Get REST APIs, count: " << restApis.size();
            Dto::ApiGateway::GetRestApisResponse response{};
            response.region = request.region;
            response.user = request.user;
            response.requestId = request.requestId;
            response.restApis = Dto::ApiGateway::Mapper::map(restApis);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::ListApiKeyCountersResponse ApiGatewayService::listApiKeyCounters(const Dto::ApiGateway::ListApiKeyCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_api_keys");
        log_debug << "List API key counters request, region:  " << request.region;

        try {

            // Get the list of API keys
            const std::vector<Database::Entity::ApiGateway::ApiKey> keys = _apiGatewayDatabase->listApiKeyCounters(request.prefix, request.pageSize, request.pageIndex, Dto::Common::SortColumnMapper::map(request.sortColumns));
            const long total = _apiGatewayDatabase->countApiKeys();

            log_trace << "Get API keys, count: " << keys.size();
            Dto::ApiGateway::ListApiKeyCountersResponse response{};
            response.total = total;
            response.apiKeys = Dto::ApiGateway::Mapper::map(keys);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::GetApiKeyCounterResponse ApiGatewayService::getApiKeyCounter(const Dto::ApiGateway::GetApiKeyCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_api_key");
        log_debug << "Get API key counter request, region:  " << request.region;

        if (!_apiGatewayDatabase->apiKeyExists(request.id)) {
            log_error << "API key does not exist, region: " << request.region << ", id: " << request.id;
            throw Core::ServiceException("API key does not exist, region: " + request.region + ", apiKey: " + request.id);
        }

        try {

            // Get the API key
            Database::Entity::ApiGateway::ApiKey key = _apiGatewayDatabase->getApiKeyById(request.id);

            Dto::ApiGateway::GetApiKeyCounterResponse response{};
            response.apiKey = Dto::ApiGateway::Mapper::map(key);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::updateApiKeyCounter(const Dto::ApiGateway::UpdateApiKeyCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "update_api_key");
        log_debug << "Update API key counter request, region:  " << request.apiKey.id;

        if (!_apiGatewayDatabase->apiKeyExists(request.apiKey.id)) {
            log_error << "API key does not exist, region: " << request.region << " id: " << request.apiKey.id;
            throw Core::ServiceException("API key does not exist, region: " + request.region + " id: " + request.apiKey.id);
        }

        try {

            // Update the API key
            Database::Entity::ApiGateway::ApiKey key = Dto::ApiGateway::Mapper::map(request.apiKey);
            key = _apiGatewayDatabase->updateApiKey(key);
            log_debug << "Api key updated, id: " + key.id;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::ListRestApiCountersResponse ApiGatewayService::listRestApiCounters(const Dto::ApiGateway::ListRestApiCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "list_rest_apis");
        log_debug << "List REST API counters request, region:  " << request.region;

        try {

            // Get the list of REST APIs
            const std::vector<Database::Entity::ApiGateway::RestApi> restApis = _apiGatewayDatabase->listRestApiCounters(request.prefix, request.pageSize, request.pageIndex, Dto::Common::SortColumnMapper::map(request.sortColumns));
            Dto::ApiGateway::ListRestApiCountersResponse response{};
            response.total = _apiGatewayDatabase->countRestApis(request.region, request.prefix);
            response.restApis = Dto::ApiGateway::Mapper::map(restApis);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::GetRestApiCounterResponse ApiGatewayService::getRestApiCounter(const Dto::ApiGateway::GetRestApiCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_rest_api");
        log_debug << "Get REST API counter request, region:  " << request.region << ", name: " << request.name;

        if (!_apiGatewayDatabase->restApiExists(request.region, request.name)) {
            log_error << "REST API does not exist, region: " << request.region << ", name: " << request.name;
            throw Core::ServiceException("REST API key does not exist, region: " + request.region + ", name: " + request.name);
        }

        try {

            // Get the API key
            Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(request.region, request.name);

            Dto::ApiGateway::GetRestApiCounterResponse response{};
            response.restApi = Dto::ApiGateway::Mapper::map(restApi);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::updateRestApiCounter(const Dto::ApiGateway::UpdateRestApiCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "update_rest_api");
        log_debug << "Update REST API counter request, region:  " << request.region << ", name: " << request.restApi.name;

        if (!_apiGatewayDatabase->restApiExists(request.region, request.restApi.name)) {
            log_error << "REST API does not exist, region: " << request.region << ", name: " << request.restApi.name;
            throw Core::ServiceException("REST API key does not exist, region: " + request.region + ", name: " + request.restApi.name);
        }

        try {

            // Get the API key
            Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(request.region, request.restApi.name);

            restApi.enabled = request.restApi.enabled;
            restApi = _apiGatewayDatabase->upsertRestApi(restApi);
            log_debug << "REST API updated, name: " + restApi.name;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::deleteRestApiCounter(const Dto::ApiGateway::DeleteRestApiCounterRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_rest_api");
        log_debug << "Delete REST API counter request, region:  " << request.region << ", name: " << request.name;

        if (!_apiGatewayDatabase->restApiExists(request.region, request.name)) {
            log_error << "REST API does not exist, region: " << request.region << ", name: " << request.name;
            throw Core::ServiceException("REST API key does not exist, region: " + request.region + ", name: " + request.name);
        }

        try {

            // Delete the API key
            const long count = _apiGatewayDatabase->deleteRestApi(request.region, request.name);
            log_debug << "REST API deleted, count: " << count;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::deleteRestApi(const Dto::ApiGateway::DeleteRestApiRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_rest_api");
        log_debug << "Delete REST API request, region:  " << request.region << ", restApiId: " << request.restApiId;

        if (!_apiGatewayDatabase->restApiExistsByRestApiId(request.restApiId)) {
            log_error << "REST API does not exist, region: " << request.region << ", restApiId: " << request.restApiId;
            throw Core::ServiceException("REST API key does not exist, region: " + request.region + ", restApiId: " + request.restApiId);
        }

        try {

            // Delete the API key
            const long count = _apiGatewayDatabase->deleteRestApi(request.restApiId);
            log_debug << "REST API deleted, count: " << count;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::CreateResourceResponse ApiGatewayService::createResource(const Dto::ApiGateway::CreateResourceRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_resource");
        log_debug << "Create resource request, region:  " << request.region << ", pathPart: " << request.pathPart;

        if (_apiGatewayDatabase->apiKeyExists(request.region, request.pathPart)) {
            log_error << "REST API exists already, region: " << request.region << " pathPart: " << request.pathPart;
            throw Core::ServiceException("REST API exists already, region: " + request.region + " pathPart: " + request.pathPart);
        }

        try {

            // Get rest api entity from request
            Database::Entity::ApiGateway::Resource resource = Dto::ApiGateway::Mapper::map(request);

            // ID
            if (resource.id.empty()) {
                resource.id = Core::AwsUtils::CreateResourceId();
            }

            // Save to the database
            //resource = _apiGatewayDatabase->createRestApi(resource);

            log_trace << "REST resource created, pathPart: " + resource.pathPart;
            return Dto::ApiGateway::Mapper::map(request, resource);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

}// namespace Awsmock::Service