//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/apigateway/CreateApiKeyRequest.h>
#include <awsmock/dto/apigateway/CreateApiKeyResponse.h>
#include <awsmock/dto/apigateway/CreateResourceRequest.h>
#include <awsmock/dto/apigateway/CreateResourceResponse.h>
#include <awsmock/dto/apigateway/CreateRestApiRequest.h>
#include <awsmock/dto/apigateway/CreateRestApiResponse.h>
#include <awsmock/dto/apigateway/DeleteApiKeyRequest.h>
#include <awsmock/dto/apigateway/DeleteResourceRequest.h>
#include <awsmock/dto/apigateway/DeleteRestApiRequest.h>
#include <awsmock/dto/apigateway/GetApiKeysRequest.h>
#include <awsmock/dto/apigateway/GetApiKeysResponse.h>
#include <awsmock/dto/apigateway/GetResourcesRequest.h>
#include <awsmock/dto/apigateway/GetResourcesResponse.h>
#include <awsmock/dto/apigateway/GetRestApisRequest.h>
#include <awsmock/dto/apigateway/GetRestApisResponse.h>
#include <awsmock/dto/apigateway/internal/DeleteRestApiCounterRequest.h>
#include <awsmock/dto/apigateway/internal/GetApiKeyCounterRequest.h>
#include <awsmock/dto/apigateway/internal/GetApiKeyCounterResponse.h>
#include <awsmock/dto/apigateway/internal/GetRestApiCounterRequest.h>
#include <awsmock/dto/apigateway/internal/GetRestApiCounterResponse.h>
#include <awsmock/dto/apigateway/internal/ListApiKeyCountersRequest.h>
#include <awsmock/dto/apigateway/internal/ListApiKeyCountersResponse.h>
#include <awsmock/dto/apigateway/internal/ListRestApiCountersRequest.h>
#include <awsmock/dto/apigateway/internal/ListRestApiCountersResponse.h>
#include <awsmock/dto/apigateway/internal/UpdateApiKeyCounterRequest.h>
#include <awsmock/dto/apigateway/internal/UpdateRestApiCounterRequest.h>
#include <awsmock/dto/apigateway/mapper/Mapper.h>
#include <awsmock/dto/common/ApiGatewayClientCommand.h>
#include <awsmock/dto/common/mapper/SortColumnMapper.h>
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/entity/apigateway/Resource.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/apigateway/ApiGatewayMongoRepository.h>

namespace Awsmock::Service {

    using std::chrono::system_clock;

    /**
     * @brief API gateway service
     *
     * @par
     * Handles all API gateway requests finally.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayService {

      public:

        /**
         * @brief Constructor
         *
         * @param ioc boost IO context
         */
        explicit ApiGatewayService(boost::asio::io_context &ioc) : _ioc(ioc) {
        }

        /**
         * @brief Creates a new API key
         *
         * @param request Api gateway create key request
         * @return API gateway create key response
         */
        [[nodiscard]]
        Dto::ApiGateway::CreateApiKeyResponse createApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const;

        /**
         * @brief Creates a new API key
         *
         * @param request Api gateway create key request
         * @return get API key response
         */
        [[nodiscard]]
        Dto::ApiGateway::GetApiKeysResponse getApiKeys(const Dto::ApiGateway::GetApiKeysRequest &request) const;

        /**
         * @brief Deletes an API gateway key
         *
         * @param request delete API key request
         */
        void deleteApiKey(const Dto::ApiGateway::DeleteApiKeyRequest &request) const;

        /**
         * @brief Creates a new REST API
         *
         * @param request create a REST API request
         * @return create REST API response
         */
        [[nodiscard]]
        Dto::ApiGateway::CreateRestApiResponse createRestApi(const Dto::ApiGateway::CreateRestApiRequest &request) const;

        /**
         * @brief Returns a List of REST APIs
         *
         * @param request get REST APIs request
         * @return list of REST APIs
         */
        Dto::ApiGateway::GetRestApisResponse getRestApis(const Dto::ApiGateway::GetRestApisRequest &request) const;

        /**
         * @brief List the API key counters
         *
         * @param request internal API key counters request
         * @return list of API key counters
         */
        [[nodiscard]]
        Dto::ApiGateway::ListApiKeyCountersResponse listApiKeyCounters(const Dto::ApiGateway::ListApiKeyCountersRequest &request) const;

        /**
         * @brief Get an API key detail
         *
         * @param request internal API key counters request
         * @return list of API key counters
         */
        [[nodiscard]]
        Dto::ApiGateway::GetApiKeyCounterResponse getApiKeyCounter(const Dto::ApiGateway::GetApiKeyCounterRequest &request) const;

        /**
         * @brief Updates an API key
         *
         * @param request internal API key counters update request
         * @return list of API key counters
         */
        void updateApiKeyCounter(const Dto::ApiGateway::UpdateApiKeyCounterRequest &request) const;

        /**
         * @brief List REST APIs
         *
         * @param request internal REST API counters request
         * @return list of REST API counters
         */
        [[nodiscard]]
        Dto::ApiGateway::ListRestApiCountersResponse listRestApiCounters(const Dto::ApiGateway::ListRestApiCountersRequest &request) const;

        /**
         * @brief Returns a REST API entity
         *
         * @param request get a REST API request
         * @return REST API response
         */
        Dto::ApiGateway::GetRestApiCounterResponse getRestApiCounter(const Dto::ApiGateway::GetRestApiCounterRequest &request) const;

        /**
         * @brief Updates a REST API entity
         *
         * @param request get a REST API update request
         */
        void updateRestApiCounter(const Dto::ApiGateway::UpdateRestApiCounterRequest &request) const;

        /**
         * @brief Deletes a REST API
         *
         * @param request delete request
         */
        void deleteRestApiCounter(const Dto::ApiGateway::DeleteRestApiCounterRequest &request) const;

        /**
         * @brief Deletes a REST API
         *
         * @param request delete request
         */
        void deleteRestApi(const Dto::ApiGateway::DeleteRestApiRequest &request) const;

        /**
         * @brief Creates a new resource
         *
         * @param request create a resource request
         * @return create resource response
         */
        Dto::ApiGateway::CreateResourceResponse createResource(const Dto::ApiGateway::CreateResourceRequest &request) const;

        /**
         * @brief Returns a list of resources
         *
         * @param request get resources request
         * @return list of resources
         */
        Dto::ApiGateway::GetResourcesResponse getResources(const Dto::ApiGateway::GetResourcesRequest &request) const;

        /**
         * @brief Creates a new resource
         *
         * @param request create a resource request
         * @return create resource response
         */
        void deleteResource(const Dto::ApiGateway::DeleteResourceRequest &request) const;

        /**
         * @brief Returns a REST API entity by its AWS ID
         *
         * @param restApiId AWS REST API ID
         * @return REST API entity
         */
        [[nodiscard]]
        Database::Entity::ApiGateway::RestApi getRestApiById(const std::string &restApiId) const;

        /**
         * @brief Adds or replaces a method on a resource
         *
         * @param restApiId AWS REST API ID
         * @param resourceId resource ID
         * @param httpMethod HTTP verb (GET, POST, ...)
         * @param apiKeyRequired whether an API key is required
         */
        void putMethod(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod, bool apiKeyRequired) const;

        /**
         * @brief Sets integration metadata on a method
         *
         * @param restApiId AWS REST API ID
         * @param resourceId resource ID
         * @param httpMethod HTTP verb
         * @param integrationType AWS, AWS_PROXY, HTTP, HTTP_PROXY, or MOCK
         * @param integrationUri Lambda ARN URI or HTTP endpoint
         * @param integrationHttpMethod HTTP method to use when calling the backend
         */
        void putIntegration(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod,
                            const std::string &integrationType, const std::string &integrationUri, const std::string &integrationHttpMethod) const;

        /**
         * @brief Returns the integration for a given REST API resource method
         *
         * @param restApiId AWS REST API ID
         * @param resourceId resource ID
         * @param httpMethod HTTP verb
         * @return ResourceMethod containing integration fields
         */
        [[nodiscard]]
        Database::Entity::ApiGateway::ResourceMethod getIntegration(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod) const;

        /**
         * @brief Removes the integration from a given REST API resource method
         *
         * @param restApiId AWS REST API ID
         * @param resourceId resource ID
         * @param httpMethod HTTP verb
         */
        void deleteIntegration(const std::string &restApiId, const std::string &resourceId, const std::string &httpMethod) const;

        /**
         * @brief Validates an API key by its raw key value
         *
         * @param keyValue the raw value from the x-api-key request header
         * @return true if an enabled key with this value exists
         */
        [[nodiscard]]
        bool validateApiKey(const std::string &keyValue) const;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

        /**
         * Database connection
         */
        std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();

        /**
         * Boost asio IO context
         */
        boost::asio::io_context &_ioc;
    };

}// namespace Awsmock::Service
