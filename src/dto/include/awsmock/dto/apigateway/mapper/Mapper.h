//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_MAPPER_H
#define AWSMOCK_DTO_API_GATEWAY_MAPPER_H

// AwsMock includes
#include <awsmock/dto/apigateway/CreateApiKeyRequest.h>
#include <awsmock/dto/apigateway/CreateApiKeyResponse.h>
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
    };

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_MAPPER_H
