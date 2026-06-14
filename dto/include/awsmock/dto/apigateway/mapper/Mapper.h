//
// Created by vogje01 on 5/10/24.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/apigateway/CreateApiKeyRequest.h>
#include <awsmock/dto/apigateway/CreateApiKeyResponse.h>
#include <awsmock/dto/apigateway/CreateResourceRequest.h>
#include <awsmock/dto/apigateway/CreateResourceResponse.h>
#include <awsmock/dto/apigateway/CreateRestApiRequest.h>
#include <awsmock/dto/apigateway/CreateRestApiResponse.h>
#include <awsmock/dto/apigateway/GetResourcesRequest.h>
#include <awsmock/dto/apigateway/GetResourcesResponse.h>
#include <awsmock/dto/apigateway/model/Authorizer.h>
#include <awsmock/dto/apigateway/model/Key.h>
#include <awsmock/dto/apigateway/model/RestApi.h>
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/entity/apigateway/Authorizer.h>
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
         * @brief Maps an API gateway key entity to a creation key response
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
         * @brief Maps a resource entity to a resource DTO
         *
         * @param resourceEntity resource entity
         * @return resource DTO
         * @see AwsMock::Dto::ApiGateway::Resource
         */
        static Resource map(const Database::Entity::ApiGateway::Resource &resourceEntity);

        /**
         * @brief Maps a resource DTO to a resource entity
         *
         * @param resourceDto resource DTO
         * @return resource entity
         */
        static Database::Entity::ApiGateway::Resource map(const Resource &resourceDto);

        /**
         * @brief Maps a map of resource entities to a map of resource DTOs
         *
         * @param resources map of resource entities keyed by resource ID
         * @return map of resource DTOs
         */
        static std::map<std::string, Resource> map(const std::map<std::string, Database::Entity::ApiGateway::Resource> &resources);

        /**
         * @brief Maps a map of resource DTOs to a map of resource entities
         *
         * @param resources map of resource DTOs keyed by resource ID
         * @return map of resource entities
         */
        static std::map<std::string, Database::Entity::ApiGateway::Resource> map(const std::map<std::string, Resource> &resources);

        /**
         * @brief Maps a list of resource entities to a list of resource DTOs
         *
         * @param resourceEntities list of resource entities
         * @return list of resource DTOs
         */
        static std::vector<Resource> map(const std::vector<Database::Entity::ApiGateway::Resource> &resourceEntities);

        /**
         * @brief Maps a get-resources request and resource map to a get-resources response
         *
         * @param request get resources request
         * @param resources map of resource entities keyed by resource ID
         * @return get resources response
         */
        static GetResourcesResponse map(const GetResourcesRequest &request, const std::map<std::string, Database::Entity::ApiGateway::Resource> &resources);

        /**
         * @brief Maps an authorizer entity to an authorizer DTO
         *
         * @param authorizerEntity authorizer entity
         * @return authorizer DTO
         */
        static Authorizer map(const Database::Entity::ApiGateway::Authorizer &authorizerEntity);

        /**
         * @brief Maps an authorizer DTO to an authorizer entity
         *
         * @param authorizerDto authorizer DTO
         * @return authorizer entity
         */
        static Database::Entity::ApiGateway::Authorizer map(const Authorizer &authorizerDto);

        /**
         * @brief Maps a map of authorizer entities to a map of authorizer DTOs
         *
         * @param authorizers map of authorizer entities keyed by authorizer ID
         * @return map of authorizer DTOs
         */
        static std::map<std::string, Authorizer> map(const std::map<std::string, Database::Entity::ApiGateway::Authorizer> &authorizers);

        /**
         * @brief Maps a map of authorizer DTOs to a map of authorizer entities
         *
         * @param authorizers map of authorizer DTOs keyed by authorizer ID
         * @return map of authorizer entities
         */
        static std::map<std::string, Database::Entity::ApiGateway::Authorizer> map(const std::map<std::string, Authorizer> &authorizers);

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
