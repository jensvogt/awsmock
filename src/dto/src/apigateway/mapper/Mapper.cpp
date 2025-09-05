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

    Key Mapper::map(const Database::Entity::ApiGateway::Key &keyEntity) {
        Key key{};
        key.id = keyEntity.id;
        key.name = keyEntity.name;
        key.customerId = keyEntity.customerId;
        key.description = keyEntity.description;
        key.enabled = keyEntity.enabled;
        key.tags = keyEntity.tags;
        return key;
    }

    std::vector<Key> Mapper::map(const std::vector<Database::Entity::ApiGateway::Key> &keyEntities) {
        std::vector<Key> keyDtos;
        for (const auto &key: keyEntities) {
            keyDtos.emplace_back(map(key));
        }
        return keyDtos;
    }

}// namespace AwsMock::Dto::ApiGateway