//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/apigateway/mapper/Mapper.h>

namespace Awsmock::Dto::ApiGateway {

    Database::Entity::ApiGateway::ApiKey Mapper::map(const CreateApiKeyRequest &request) {
        Database::Entity::ApiGateway::ApiKey keyEntity;
        keyEntity.region = request.region;
        keyEntity.name = request.name;
        keyEntity.keyValue = request.value;
        keyEntity.description = request.description;
        keyEntity.enabled = request.enabled;
        keyEntity.customerId = request.customerId;
        keyEntity.generateDistinct = request.generateDistinct;
        keyEntity.tags = request.tags;
        return keyEntity;
    }

    CreateApiKeyResponse Mapper::map(const CreateApiKeyRequest &request, const Database::Entity::ApiGateway::ApiKey &keyEntity) {
        CreateApiKeyResponse response{};
        response.region = request.region;
        response.user = request.user;
        response.requestId = request.requestId;
        response.region = keyEntity.region;
        response.id = keyEntity.id;
        response.name = keyEntity.name;
        response.customerId = keyEntity.customerId;
        response.description = keyEntity.description;
        response.enabled = keyEntity.enabled;
        response.tags = keyEntity.tags;
        response.created = keyEntity.created;
        response.modified = keyEntity.modified;
        return response;
    }

    Key Mapper::map(const Database::Entity::ApiGateway::ApiKey &keyEntity) {
        Key key{};
        key.id = keyEntity.id;
        key.name = keyEntity.name;
        key.customerId = keyEntity.customerId;
        key.description = keyEntity.description;
        key.enabled = keyEntity.enabled;
        key.tags = keyEntity.tags;
        key.value = keyEntity.keyValue;
        key.created = keyEntity.created;
        key.modified = keyEntity.modified;
        return key;
    }

    std::vector<Key> Mapper::map(const std::vector<Database::Entity::ApiGateway::ApiKey> &keyEntities) {
        std::vector<Key> keyDtos;
        for (const auto &key: keyEntities) {
            keyDtos.emplace_back(map(key));
        }
        return keyDtos;
    }

    Database::Entity::ApiGateway::ApiKey Mapper::map(const Key &keyDto) {
        Database::Entity::ApiGateway::ApiKey keyEntity{};
        keyEntity.id = keyDto.id;
        keyEntity.name = keyDto.name;
        keyEntity.customerId = keyDto.customerId;
        keyEntity.description = keyDto.description;
        keyEntity.enabled = keyDto.enabled;
        keyEntity.tags = keyDto.tags;
        keyEntity.keyValue = keyDto.value;
        keyEntity.created = keyDto.created;
        keyEntity.modified = keyDto.modified;
        return keyEntity;
    }

    // ========================================================================================================================
    // Resource
    // ========================================================================================================================
    Database::Entity::ApiGateway::Resource Mapper::map(const CreateResourceRequest &request) {
        Database::Entity::ApiGateway::Resource resourceEntity;
        resourceEntity.pathPart = request.pathPart;
        return resourceEntity;
    }

    CreateResourceResponse Mapper::map(const CreateResourceRequest &request, const Database::Entity::ApiGateway::Resource &resourceEntity) {
        CreateResourceResponse response{};
        response.region = request.region;
        response.user = request.user;
        response.requestId = request.requestId;
        response.id = resourceEntity.id;
        response.parentId = resourceEntity.parentId;
        response.path = resourceEntity.path;
        response.pathPart = resourceEntity.pathPart;
        response.url = resourceEntity.url;
        response.created = resourceEntity.created;
        response.modified = resourceEntity.modified;
        return response;
    }

    Resource Mapper::map(const Database::Entity::ApiGateway::Resource &resourceEntity) {
        Resource resource{};
        resource.id = resourceEntity.id;
        resource.parentId = resourceEntity.parentId;
        resource.path = resourceEntity.path;
        resource.pathPart = resourceEntity.pathPart;
        resource.url = resourceEntity.url;
        resource.created = resourceEntity.created;
        resource.modified = resourceEntity.modified;
        for (const auto &[key, methodEntity]: resourceEntity.resourceMethods) {
            ResourceMethod method{};
            method.httpMethod = methodEntity.httpMethod;
            method.apiKeyRequired = methodEntity.apiKeyRequired;
            method.integrationType = methodEntity.integrationType;
            method.integrationUri = methodEntity.integrationUri;
            method.integrationHttpMethod = methodEntity.integrationHttpMethod;
            resource.resourceMethods[key] = method;
        }
        return resource;
    }

    Database::Entity::ApiGateway::Resource Mapper::map(const Resource &resourceDto) {
        Database::Entity::ApiGateway::Resource resourceEntity{};
        resourceEntity.id = resourceDto.id;
        resourceEntity.parentId = resourceDto.parentId;
        resourceEntity.path = resourceDto.path;
        resourceEntity.pathPart = resourceDto.pathPart;
        resourceEntity.url = resourceDto.url;
        resourceEntity.created = resourceDto.created;
        resourceEntity.modified = resourceDto.modified;
        for (const auto &[key, methodDto]: resourceDto.resourceMethods) {
            Database::Entity::ApiGateway::ResourceMethod method{};
            method.httpMethod = methodDto.httpMethod;
            method.apiKeyRequired = methodDto.apiKeyRequired;
            method.integrationType = methodDto.integrationType;
            method.integrationUri = methodDto.integrationUri;
            method.integrationHttpMethod = methodDto.integrationHttpMethod;
            resourceEntity.resourceMethods[key] = method;
        }
        return resourceEntity;
    }

    std::map<std::string, Resource> Mapper::map(const std::map<std::string, Database::Entity::ApiGateway::Resource> &resources) {
        std::map<std::string, Resource> result;
        for (const auto &[key, resourceEntity]: resources) {
            result[key] = map(resourceEntity);
        }
        return result;
    }

    std::map<std::string, Database::Entity::ApiGateway::Resource> Mapper::map(const std::map<std::string, Resource> &resources) {
        std::map<std::string, Database::Entity::ApiGateway::Resource> result;
        for (const auto &[key, resourceDto]: resources) {
            result[key] = map(resourceDto);
        }
        return result;
    }

    std::vector<Resource> Mapper::map(const std::vector<Database::Entity::ApiGateway::Resource> &resourceEntities) {
        std::vector<Resource> result;
        result.reserve(resourceEntities.size());
        for (const auto &resourceEntity: resourceEntities) {
            result.emplace_back(map(resourceEntity));
        }
        return result;
    }

    GetResourcesResponse Mapper::map(const GetResourcesRequest &request, const std::map<std::string, Database::Entity::ApiGateway::Resource> &resources) {
        GetResourcesResponse response{};
        response.region = request.region;
        response.user = request.user;
        response.requestId = request.requestId;
        for (const auto &[id, resourceEntity]: resources) {
            response.resources.emplace_back(map(resourceEntity));
        }
        return response;
    }

    // ========================================================================================================================
    // Authorizer
    // ========================================================================================================================
    Authorizer Mapper::map(const Database::Entity::ApiGateway::Authorizer &authorizerEntity) {
        Authorizer authorizer{};
        authorizer.id = authorizerEntity.id;
        authorizer.name = authorizerEntity.name;
        authorizer.type = authorizerEntity.type;
        authorizer.authType = authorizerEntity.authType;
        authorizer.authorizerUri = authorizerEntity.authorizerUri;
        authorizer.identitySource = authorizerEntity.identitySource;
        authorizer.authorizerResultTtlInSeconds = authorizerEntity.authorizerResultTtlInSeconds;
        return authorizer;
    }

    Database::Entity::ApiGateway::Authorizer Mapper::map(const Authorizer &authorizerDto) {
        Database::Entity::ApiGateway::Authorizer authorizerEntity{};
        authorizerEntity.id = authorizerDto.id;
        authorizerEntity.name = authorizerDto.name;
        authorizerEntity.type = authorizerDto.type;
        authorizerEntity.authType = authorizerDto.authType;
        authorizerEntity.authorizerUri = authorizerDto.authorizerUri;
        authorizerEntity.identitySource = authorizerDto.identitySource;
        authorizerEntity.authorizerResultTtlInSeconds = authorizerDto.authorizerResultTtlInSeconds;
        return authorizerEntity;
    }

    std::map<std::string, Authorizer> Mapper::map(const std::map<std::string, Database::Entity::ApiGateway::Authorizer> &authorizers) {
        std::map<std::string, Authorizer> result;
        for (const auto &[key, authorizerEntity]: authorizers) {
            result[key] = map(authorizerEntity);
        }
        return result;
    }

    std::map<std::string, Database::Entity::ApiGateway::Authorizer> Mapper::map(const std::map<std::string, Authorizer> &authorizers) {
        std::map<std::string, Database::Entity::ApiGateway::Authorizer> result;
        for (const auto &[key, authorizerDto]: authorizers) {
            result[key] = map(authorizerDto);
        }
        return result;
    }

    // ========================================================================================================================
    // REST API
    // ========================================================================================================================
    Database::Entity::ApiGateway::RestApi Mapper::map(const CreateRestApiRequest &request) {
        Database::Entity::ApiGateway::RestApi restApiEntity;
        restApiEntity.region = request.region;
        restApiEntity.name = request.name;
        restApiEntity.apiKeySource = Database::Entity::ApiGateway::ApiKeySourceTypeFromString(ApiKeySourceTypeToString(request.apiKeySource));
        restApiEntity.description = request.description;
        restApiEntity.binaryMediaTypes = request.binaryMediaTypes;
        restApiEntity.cloneFrom = request.cloneFrom;
        restApiEntity.disableExecuteApiEndpoint = request.disableExecuteApiEndpoint;
        restApiEntity.minimumCompressionSize = request.minimumCompressionSize;
        restApiEntity.policy = request.policy;
        restApiEntity.version = request.version;
        restApiEntity.tags = request.tags;
        return restApiEntity;
    }

    CreateRestApiResponse Mapper::map(const CreateRestApiRequest &request, const Database::Entity::ApiGateway::RestApi &restApiEntity) {
        CreateRestApiResponse response{};
        response.region = request.region;
        response.user = request.user;
        response.requestId = request.requestId;
        response.name = restApiEntity.name;
        response.id = restApiEntity.id;
        response.apiKeySource = ApiKeySourceTypeFromString(Database::Entity::ApiGateway::ApiKeySourceTypeToString(restApiEntity.apiKeySource));
        response.description = restApiEntity.description;
        response.policy = restApiEntity.policy;
        response.rootResourceId = restApiEntity.rootResourceId;
        response.binaryMediaTypes = restApiEntity.binaryMediaTypes;
        response.tags = restApiEntity.tags;
        response.warnings = restApiEntity.warnings;
        response.created = restApiEntity.created;
        return response;
    }

    RestApi Mapper::map(const Database::Entity::ApiGateway::RestApi &restApiEntity) {
        RestApi restApi{};
        restApi.id = restApiEntity.id;
        restApi.name = restApiEntity.name;
        restApi.apiKeySource = ApiKeySourceTypeFromString(Database::Entity::ApiGateway::ApiKeySourceTypeToString(restApiEntity.apiKeySource));
        restApi.description = restApiEntity.description;
        restApi.policy = restApiEntity.policy;
        restApi.rootResourceId = restApiEntity.rootResourceId;
        restApi.binaryMediaTypes = restApiEntity.binaryMediaTypes;
        restApi.tags = restApiEntity.tags;
        restApi.enabled = restApiEntity.enabled;
        // restApi.customerId = restApiEntity.customerId;
        restApi.warnings = restApiEntity.warnings;
        restApi.created = restApiEntity.created;
        restApi.modified = restApiEntity.modified;
        restApi.authorizers = map(restApiEntity.authorizers);
        restApi.resources = map(restApiEntity.resources);
        return restApi;
    }

    std::vector<RestApi> Mapper::map(const std::vector<Database::Entity::ApiGateway::RestApi> &restApiEntities) {
        std::vector<RestApi> restApiDtos;
        for (const auto &restApi: restApiEntities) {
            restApiDtos.emplace_back(map(restApi));
        }
        return restApiDtos;
    }

}// namespace Awsmock::Dto::ApiGateway