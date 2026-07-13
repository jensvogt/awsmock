//
// Created by vogje01 on 9/2/25.
//

#include <awsmock/service/apigateway/ApiGatewayService.h>

namespace Awsmock::Service {

    Database::Entity::ApiGateway::Authorizer ApiGatewayService::createAuthorizer(const std::string &restApiId, const std::string &name,
                                                                                 const std::string &type, const std::string &authorizerUri,
                                                                                 const std::string &identitySource, const std::int64_t ttl) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_authorizer");
        log_debug << "Create authorizer, restApiId: " << restApiId << ", name: " << name;

        Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(restApiId);

        Database::Entity::ApiGateway::Authorizer authorizer;
        authorizer.id = Core::StringUtils::GenerateRandomHexString(6);
        authorizer.name = name;
        authorizer.type = type;
        authorizer.authorizerUri = authorizerUri;
        authorizer.identitySource = identitySource;
        authorizer.authorizerResultTtlInSeconds = ttl;

        restApi.authorizers[authorizer.id] = authorizer;
        restApi = _apiGatewayDatabase->upsertRestApi(restApi);
        log_info << "Authorizer created, restApiId: " << restApi.id << ", authorizerId: " << authorizer.id;
        return authorizer;
    }

    Dto::ApiGateway::CreateApiKeyResponse ApiGatewayService::createApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_api_key");
        log_debug << "Create API key request, region:  " << request.region << " customerId: " << request.customerId;

        if (_apiGatewayDatabase->apiKeyExists(request.region, request.name)) {
            log_error << "API key exists already, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("API key exists already, region: " + request.region + " name: " + request.name);
        }

        try {

            // Generate API key
            Database::Entity::ApiGateway::ApiKey key = Dto::ApiGateway::Mapper::map(request);

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

    Dto::ApiGateway::CreateDeploymentResponse ApiGatewayService::createDeployment(const std::string &restApiId, const std::string &stageName) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_deployment");
        log_debug << "Create deployment, restApiId: " << restApiId << " stageName: " << stageName;

        Dto::ApiGateway::CreateDeploymentResponse response;
        response.id = Core::AwsUtils::CreateApiKeyId();
        response.createdDate = system_clock::now();
        log_info << "Deployment created, restApiId: " << restApiId << " stageName: " << stageName << " deploymentId: " << response.id;
        return response;
    }

    Dto::ApiGateway::CreateDeploymentResponse ApiGatewayService::updateDeployment(const std::string &restApiId, const std::string &deploymentId, const std::string &description) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "update_deployment");
        log_debug << "Update deployment, restApiId: " << restApiId << " deploymentId: " << deploymentId;

        Dto::ApiGateway::CreateDeploymentResponse response;
        response.id = deploymentId;
        response.description = description;
        response.createdDate = system_clock::now();
        log_info << "Deployment updated, restApiId: " << restApiId << " deploymentId: " << deploymentId;
        return response;
    }

    void ApiGatewayService::deleteDeployment(const std::string &restApiId, const std::string &deploymentId) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_deployment");
        log_info << "Deployment deleted, restApiId: " << restApiId << " deploymentId: " << deploymentId;
    }

    Dto::ApiGateway::CreateUsagePlanResponse ApiGatewayService::createUsagePlan(const Dto::ApiGateway::CreateUsagePlanRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_usage_plan");
        log_debug << "Create usage plan, name: " << request.name;

        try {
            Database::Entity::ApiGateway::UsagePlan plan;
            plan.region = request.region;
            plan.id = Core::AwsUtils::CreateApiKeyId();
            plan.name = request.name;
            plan.description = request.description;
            plan.quota.limit = request.quota.limit;
            plan.quota.offset = request.quota.offset;
            plan.quota.period = request.quota.period;
            plan.throttle.burstLimit = request.throttle.burstLimit;
            plan.throttle.rateLimit = request.throttle.rateLimit;
            plan.tags = request.tags;
            plan = _apiGatewayDatabase->createUsagePlan(plan);

            Dto::ApiGateway::CreateUsagePlanResponse response;
            response.copyMetadata(request);
            response.id = plan.id;
            response.name = plan.name;
            response.description = plan.description;
            response.quotaLimit = plan.quota.limit;
            response.quotaOffset = plan.quota.offset;
            response.quotaPeriod = plan.quota.period;
            response.throttleBurstLimit = plan.throttle.burstLimit;
            response.throttleRateLimit = plan.throttle.rateLimit;
            response.tags = plan.tags;
            log_info << "Usage plan created, id: " << plan.id;
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::CreateUsagePlanKeyResponse ApiGatewayService::createUsagePlanKey(const Dto::ApiGateway::CreateUsagePlanKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "create_usage_plan_key");
        log_debug << "Create usage plan key, usagePlanId: " << request.usagePlanId << " keyId: " << request.keyId;

        if (!_apiGatewayDatabase->apiKeyExists(request.keyId)) {
            log_error << "API key not found, keyId: " << request.keyId;
            throw Core::NotFoundException("API key not found, keyId: " + request.keyId);
        }

        try {
            Database::Entity::ApiGateway::ApiKey key = _apiGatewayDatabase->getApiKeyById(request.keyId);
            key.usagePlanId = request.usagePlanId;
            key = _apiGatewayDatabase->updateApiKey(key);

            Dto::ApiGateway::CreateUsagePlanKeyResponse response;
            response.copyMetadata(request);
            response.id = key.id;
            response.name = key.name;
            response.type = request.keyType;
            response.value = key.keyValue;
            log_info << "Usage plan key created, usagePlanId: " << request.usagePlanId << " keyId: " << key.id;
            return response;

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

    void ApiGatewayService::enableApiKey(const Dto::ApiGateway::EnableApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "enable_api_key");
        log_debug << "Enable API key request, region: " << request.region << " keyId: " << request.keyId;

        if (!_apiGatewayDatabase->apiKeyExists(request.keyId)) {
            log_error << "API key does not exist, region: " << request.region << " keyId: " << request.keyId;
            throw Core::NotFoundException("API key does not exist, keyId: " + request.keyId);
        }

        try {

            Database::Entity::ApiGateway::ApiKey key = _apiGatewayDatabase->getApiKeyById(request.keyId);
            key.enabled = true;
            _apiGatewayDatabase->updateApiKey(key);
            log_info << "API key enabled, keyId: " << request.keyId;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::disableApiKey(const Dto::ApiGateway::DisableApiKeyRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "disable_api_key");
        log_debug << "Disable API key request, region: " << request.region << " keyId: " << request.keyId;

        if (!_apiGatewayDatabase->apiKeyExists(request.keyId)) {
            log_error << "API key does not exist, region: " << request.region << " keyId: " << request.keyId;
            throw Core::NotFoundException("API key does not exist, keyId: " + request.keyId);
        }

        try {

            Database::Entity::ApiGateway::ApiKey key = _apiGatewayDatabase->getApiKeyById(request.keyId);
            key.enabled = false;
            _apiGatewayDatabase->updateApiKey(key);
            log_info << "API key disabled, keyId: " << request.keyId;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::deleteUsagePlan(const Dto::ApiGateway::DeleteUsagePlanRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_usage_plan");
        log_debug << "Delete usage plan request, region: " << request.region << " usagePlanId: " << request.usagePlanId;

        if (!_apiGatewayDatabase->usagePlanExists(request.usagePlanId)) {
            log_error << "Usage plan does not exist, region: " << request.region << " usagePlanId: " << request.usagePlanId;
            throw Core::ServiceException("Usage plan does not exist, region: " + request.region + " usagePlanId: " + request.usagePlanId);
        }

        try {
            _apiGatewayDatabase->deleteUsagePlan(request.usagePlanId);
            log_trace << "Usage plan deleted, id: " << request.usagePlanId;
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

            // Endpoint URL
            if (restApi.endpointUrl.empty()) {
                restApi.endpointUrl = Core::AwsUtils::CreateApiGatewayRestApiUrl(restApi.id, restApi.region);
            }

            // Root resourceId and root resource entry
            if (restApi.rootResourceId.empty()) {
                restApi.rootResourceId = Core::AwsUtils::CreateRestApiId();
            }
            if (!restApi.resources.contains(restApi.rootResourceId)) {
                Database::Entity::ApiGateway::Resource rootResource;
                rootResource.id = restApi.rootResourceId;
                rootResource.path = "/";
                rootResource.pathPart = "/";
                rootResource.region = restApi.region;
                restApi.resources[restApi.rootResourceId] = rootResource;
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

        if (!_apiGatewayDatabase->apiKeyExists(request.keyId)) {
            log_error << "API key does not exist, region: " << request.region << ", keyId: " << request.keyId;
            throw Core::ServiceException("API key does not exist, region: " + request.region + ", keyId: " + request.keyId);
        }

        try {

            // Get the API key
            Database::Entity::ApiGateway::ApiKey key = _apiGatewayDatabase->getApiKeyById(request.keyId);

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
            restApi.description = request.restApi.description;
            restApi.apiKeySource = Database::Entity::ApiGateway::ApiKeySourceTypeFromString(ApiKeySourceTypeToString(request.restApi.apiKeySource));
            restApi.resources = Dto::ApiGateway::Mapper::map(request.restApi.resources);
            restApi.modified = Core::DateTimeUtils::LocalDateTimeNow();
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
        log_debug << "Create resource request, region:  " << request.region << ", restApiId:" << request.restApiId << ", parentId: " << request.parentId << ", pathPart: " << request.pathPart;

        if (!_apiGatewayDatabase->restApiExistsByRestApiId(request.restApiId)) {
            log_error << "REST API does not exist, region: " << request.region << " restApiId: " << request.restApiId;
            throw Core::ServiceException("REST API does not exist already, region: " + request.region + " restApiId: " + request.restApiId);
        }

        try {

            // Get the rest api
            Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(request.restApiId);

            // Get rest api entity from request
            Database::Entity::ApiGateway::Resource resource = Dto::ApiGateway::Mapper::map(request);

            // Check whether it already exists
            if (restApi.resourceExists(resource.pathPart)) {
                log_error << "REST API resource exist already, region: " << request.region << " restApiId: " << request.restApiId << ", pathPart: " << request.pathPart;
                throw Core::ServiceException("REST API does not exist already, region: " + request.region + ", restApiId: " + request.restApiId + ", pathPart: " + request.pathPart);
            }

            // ID
            if (resource.id.empty()) {
                resource.id = Core::AwsUtils::CreateResourceId();
            }

            // Parent ID and path
            resource.parentId = request.parentId;
            if (!request.parentId.empty() && restApi.resources.contains(request.parentId)) {
                const auto &parentPath = restApi.resources.at(request.parentId).path;
                resource.path = (parentPath == "/" ? "" : parentPath) + "/" + resource.pathPart;
            } else {
                resource.path = "/" + resource.pathPart;
            }

            // URL
            if (restApi.endpointUrl.empty()) {
                restApi.endpointUrl = Core::AwsUtils::CreateApiGatewayRestApiUrl(restApi.id, restApi.region);
            }
            resource.url = restApi.endpointUrl + resource.path;

            // Save to the database
            restApi.resources[resource.id] = resource;
            restApi = _apiGatewayDatabase->upsertRestApi(restApi);
            log_trace << "REST resource created, restApi:" << restApi.id << ", pathPart: " + resource.pathPart;

            return Dto::ApiGateway::Mapper::map(request, resource);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    Dto::ApiGateway::GetResourcesResponse ApiGatewayService::getResources(const Dto::ApiGateway::GetResourcesRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_resources");
        log_debug << "Get resources request, region:  " << request.region << ", restApiId:" << request.restApiId;

        if (!_apiGatewayDatabase->restApiExistsByRestApiId(request.restApiId)) {
            log_error << "REST API does not exist, region: " << request.region << " restApiId: " << request.restApiId;
            throw Core::ServiceException("REST API does not exist already, region: " + request.region + " restApiId: " + request.restApiId);
        }

        try {

            // Get the rest api
            const Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(request.restApiId);

            return Dto::ApiGateway::Mapper::map(request, restApi.resources);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    void ApiGatewayService::deleteResource(const Dto::ApiGateway::DeleteResourceRequest &request) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_resource");
        log_debug << "Delete resource request, region:  " << request.region << ", restApiId:" << request.restApiId << ", resourceId: " << request.resourceId;

        if (!_apiGatewayDatabase->restApiExistsByRestApiId(request.restApiId)) {
            log_error << "REST API does not exist, region: " << request.region << " restApiId: " << request.restApiId;
            throw Core::ServiceException("REST API does not exist already, region: " + request.region + " restApiId: " + request.restApiId);
        }

        try {

            // Get the rest api
            Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(request.restApiId);

            // Get rest api entity from request
            restApi.resources.erase(request.resourceId);

            // Save to the database
            restApi = _apiGatewayDatabase->upsertRestApi(restApi);
            log_trace << "REST resource deleted, restApi:" << restApi.id << ", restApiId: " + restApi.id;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }
    Database::Entity::ApiGateway::RestApi ApiGatewayService::getRestApiById(const std::string &restApiId) const {
        return _apiGatewayDatabase->getRestApi(restApiId);
    }

    void ApiGatewayService::putMethod(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod, const bool apiKeyRequired) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "put_method");
        log_debug << "Put method, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;

        Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(restApiId);
        if (!restApi.resources.contains(resourceId)) {
            throw Core::ServiceException("Resource not found, resourceId: " + resourceId);
        }

        // Get the resource
        auto &resource = restApi.resources.at(resourceId);

        // Create method
        Database::Entity::ApiGateway::ResourceMethod method;
        method.httpMethod = httpMethod;
        method.apiKeyRequired = apiKeyRequired;
        resource.resourceMethods[httpMethod] = method;
        restApi = _apiGatewayDatabase->upsertRestApi(restApi);
        log_debug << "Put method created, restApiId: " << restApi.id << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;
    }

    void ApiGatewayService::putIntegration(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod,
                                           const std::string &integrationType, const std::string &integrationUri, const std::string &integrationHttpMethod) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "put_integration");
        log_debug << "Put integration, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod << ", type: " << integrationType;

        Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(restApiId);
        if (!restApi.resources.contains(resourceId)) {
            throw Core::ServiceException("Resource not found, resourceId: " + resourceId);
        }
        auto &resource = restApi.resources.at(resourceId);
        auto &method = resource.resourceMethods[httpMethod];
        method.httpMethod = httpMethod;
        method.integrationType = integrationType;
        method.integrationUri = integrationUri;
        method.integrationHttpMethod = integrationHttpMethod;
        restApi = _apiGatewayDatabase->upsertRestApi(restApi);
        log_debug << "Put integration created, restApiId: " << restApi.id << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod << ", type: " << integrationType;
    }

    Database::Entity::ApiGateway::ResourceMethod ApiGatewayService::getIntegration(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "get_integration");
        log_debug << "Get integration, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;

        const Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(restApiId);
        if (!restApi.resources.contains(resourceId)) {
            throw Core::NotFoundException("Resource not found, resourceId: " + resourceId);
        }
        const auto &resource = restApi.resources.at(resourceId);
        const auto it = resource.resourceMethods.find(httpMethod);
        if (it == resource.resourceMethods.end()) {
            throw Core::NotFoundException("Method not found, httpMethod: " + httpMethod);
        }
        return it->second;
    }

    void ApiGatewayService::deleteIntegration(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod) const {
        Monitoring::MonitoringTimer measure(API_GATEWAY_SERVICE_TIMER, API_GATEWAY_SERVICE_COUNTER, "action", "delete_integration");
        log_debug << "Delete integration, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;

        Database::Entity::ApiGateway::RestApi restApi = _apiGatewayDatabase->getRestApi(restApiId);
        if (!restApi.resources.contains(resourceId)) {
            throw Core::NotFoundException("Resource not found, resourceId: " + resourceId);
        }
        auto &resource = restApi.resources.at(resourceId);
        const auto it = resource.resourceMethods.find(httpMethod);
        if (it == resource.resourceMethods.end()) {
            throw Core::NotFoundException("Method not found, httpMethod: " + httpMethod);
        }
        auto &method = it->second;
        method.integrationType.clear();
        method.integrationUri.clear();
        method.integrationHttpMethod.clear();
        std::ignore = _apiGatewayDatabase->upsertRestApi(restApi);
        log_debug << "Integration deleted, restApiId: " << restApiId << ", resourceId: " << resourceId << ", httpMethod: " << httpMethod;
    }

    bool ApiGatewayService::validateApiKey(const std::string &keyValue) const {
        const auto keys = _apiGatewayDatabase->listApiKeys();
        return std::ranges::any_of(keys, [&keyValue](const auto &k) {
            return k.keyValue == keyValue && k.enabled;
        });
    }

}// namespace Awsmock::Service
