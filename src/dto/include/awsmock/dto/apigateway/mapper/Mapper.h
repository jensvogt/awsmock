//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_MAPPER_H
#define AWSMOCK_DTO_API_GATEWAY_MAPPER_H

// AwsMock includes
#include <awsmock/dto/apigateway/CreateApiKeyRequest.h>
#include <awsmock/dto/apigateway/CreateApiKeyResponse.h>
#include <awsmock/dto/apigateway/model/Key.h>
#include <awsmock/entity/apigateway/Key.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps an API gateway key request to an API gateway key entity
         *
         * @param request API create key request
         * @return API key entity
         * @see AwsMock::Dto::ApiGateway::CreateKeyRequest
         */
        static Database::Entity::ApiGateway::Key map(const CreateApiKeyRequest &request);

        /**
         * @brief Maps an API gateway key entity to a create key response
         *
         * @param request
         * @param keyEntity API key entity
         * @return API key DTO
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static CreateApiKeyResponse map(const CreateApiKeyRequest &request, const Database::Entity::ApiGateway::Key &keyEntity);

        /**
         * @brief Maps an API gateway key entity to a key DTO
         *
         * @param keyEntity API key entity
         * @return API key DTO
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static Key map(const Database::Entity::ApiGateway::Key &keyEntity);

        /**
         * @brief Maps a list of API gateway key entities to a list of key DTOs
         *
         * @param keyEntities list of API key entities
         * @return list of API key DTOs
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static std::vector<Key> map(const std::vector<Database::Entity::ApiGateway::Key> &keyEntities);

        /**
         * @brief Maps an API gateway key DTO to a key entity
         *
         * @param keyDto API key DTO
         * @return API key entity
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static Database::Entity::ApiGateway::Key map(const Key &keyDto);
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_MAPPER_H
