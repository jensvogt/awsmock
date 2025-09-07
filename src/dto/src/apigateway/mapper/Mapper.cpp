//
// Created by vogje01 on 5/10/24.
//

#include "awsmock/dto/apigateway/CreateRestApiRequest.h"
#include "awsmock/entity/apigateway/RestApi.h"


#include <awsmock/dto/apigateway/mapper/Mapper.h>

namespace AwsMock::Dto::ApiGateway {

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
    // REST API
    // ========================================================================================================================
    Database::Entity::ApiGateway::RestApi Mapper::map(const CreateRestApiRequest &request) {
        Database::Entity::ApiGateway::RestApi restApiEntity;
        restApiEntity.region = request.region;
        restApiEntity.name = request.name;
        restApiEntity.apiKeySource = ApiKeySourceTypeToString(request.apiKeySource);
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
        response.apiKeySource = ApiKeySourceTypeFromString(restApiEntity.apiKeySource);
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
        restApi.name = restApiEntity.name;
        restApi.apiKeySource = ApiKeySourceTypeFromString(restApiEntity.apiKeySource);
        restApi.description = restApiEntity.description;
        restApi.policy = restApiEntity.policy;
        restApi.rootResourceId = restApiEntity.rootResourceId;
        restApi.binaryMediaTypes = restApiEntity.binaryMediaTypes;
        restApi.tags = restApiEntity.tags;
        restApi.warnings = restApiEntity.warnings;
        restApi.created = restApiEntity.created;
        return restApi;
    }

    std::vector<RestApi> Mapper::map(const std::vector<Database::Entity::ApiGateway::RestApi> &restApiEntities) {
        std::vector<RestApi> restApiDtos;
        for (const auto &restApi: restApiEntities) {
            restApiDtos.emplace_back(map(restApi));
        }
        return restApiDtos;
    }

}// namespace AwsMock::Dto::ApiGateway