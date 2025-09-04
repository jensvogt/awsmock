//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/apigateway/mapper/Mapper.h>

namespace AwsMock::Dto::ApiGateway {

    Database::Entity::ApiGateway::Key Mapper::map(const CreateApiKeyRequest &request) {
        Database::Entity::ApiGateway::Key keyEntity;
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

    CreateApiKeyResponse Mapper::map(const CreateApiKeyRequest &request, const Database::Entity::ApiGateway::Key &keyEntity) {
        CreateApiKeyResponse response{};
        response.region = request.region;
        response.user = request.user;
        response.requestId = request.requestId;
        response.region = keyEntity.region;
        response.customerId = keyEntity.customerId;
        response.name = keyEntity.name;
        response.description = keyEntity.description;
        response.enabled = keyEntity.enabled;
        response.tags = keyEntity.tags;
        return response;
    }
}// namespace AwsMock::Dto::ApiGateway