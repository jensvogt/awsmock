//
// Created by vogje01 on 5/10/24.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/apigateway/CreateApiKeyRequest.h>
#include <awsmock/dto/apigateway/CreateApiKeyResponse.h>
#include <awsmock/dto/apigateway/CreateResourceRequest.h>
#include <awsmock/dto/apigateway/CreateResourceResponse.h>
#include <awsmock/dto/apigateway/CreateRestApiResponse.h>
#include <awsmock/dto/apigateway/model/Key.h>
#include <awsmock/dto/apigateway/model/RestApi.h>
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/entity/apigateway/Resource.h>
#include <awsmock/entity/apigateway/RestApi.h>

namespace Awsmock::Dto::ApiGateway {

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
        static Database::Entity::ApiGateway::ApiKey map(const CreateApiKeyRequest &request);

        /**
         * @brief Maps an API gateway key entity to a create key response
         *
         * @param request
         * @param keyEntity API key entity
         * @return API key DTO
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static CreateApiKeyResponse map(const CreateApiKeyRequest &request, const Database::Entity::ApiGateway::ApiKey &keyEntity);

        /**
         * @brief Maps an API gateway key entity to a key DTO
         *
         * @param keyEntity API key entity
         * @return API key DTO
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static Key map(const Database::Entity::ApiGateway::ApiKey &keyEntity);

        /**
         * @brief Maps a list of API gateway key entities to a list of key DTOs
         *
         * @param keyEntities list of API key entities
         * @return list of API key DTOs
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static std::vector<Key> map(const std::vector<Database::Entity::ApiGateway::ApiKey> &keyEntities);

        /**
         * @brief Maps an API gateway key DTO to a key entity
         *
         * @param keyDto API key DTO
         * @return API key entity
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static Database::Entity::ApiGateway::ApiKey map(const Key &keyDto);

        /**
         * @brief Maps a create resource request to a resource entity
         *
         * @param request create resource request
         * @return resource entity
         * @see AwsMock::Dto::ApiGateway::CreateResourceRequest
         */
        static Database::Entity::ApiGateway::Resource map(const CreateResourceRequest &request);

        /**
         * @brief Maps a create resource request and resource entity to a create resource response
         *
         * @param request create resource request
         * @param resourceEntity resource entity
         * @return create resource response
         * @see AwsMock::Dto::ApiGateway::CreateResourceResponse
         */
        static CreateResourceResponse map(const CreateResourceRequest &request, const Database::Entity::ApiGateway::Resource &resourceEntity);

        /**
         * @brief Maps a REST API request to a REST API entity
         *
         * @param request REST API create request
         * @return REST API entity
         * @see AwsMock::Dto::ApiGateway::CreateRestApiRequest
         */
        static Database::Entity::ApiGateway::RestApi map(const CreateRestApiRequest &request);

        /**
         * @brief Maps a REST API entity to a 'create' REST API response
         *
         * @param request the REST API request
         * @param restApiEntity REST API entity
         * @return API key DTO
         * @see AwsMock::Dto::ApiGateway::Key
         */
        static CreateRestApiResponse map(const CreateRestApiRequest &request, const Database::Entity::ApiGateway::RestApi &restApiEntity);

        /**
         * @brief Maps an REST API entity to a REST API DTO
         *
         * @param restApiEntity REST API entity
         * @return REST API DTO
         * @see AwsMock::Dto::ApiGateway::RestApi
         */
        static RestApi map(const Database::Entity::ApiGateway::RestApi &restApiEntity);

        /**
         * @brief Maps a list of REST API entities to a list of REST API DTOs
         *
         * @param restApiEntities list of REST API entities
         * @return list of REST API DTOs
         * @see AwsMock::Dto::ApiGateway::RestApi
         */
        static std::vector<RestApi> map(const std::vector<Database::Entity::ApiGateway::RestApi> &restApiEntities);
    };

}// namespace Awsmock::Dto::ApiGateway
