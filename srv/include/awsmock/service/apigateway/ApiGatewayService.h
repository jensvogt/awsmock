//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/apigateway/CreateApiKeyRequest.h>
#include <awsmock/dto/apigateway/CreateApiKeyResponse.h>
#include <awsmock/dto/apigateway/CreateRestApiRequest.h>
#include <awsmock/dto/apigateway/CreateRestApiResponse.h>
#include <awsmock/dto/apigateway/DeleteApiKeyRequest.h>
#include <awsmock/dto/apigateway/GetApiKeysRequest.h>
#include <awsmock/dto/apigateway/GetApiKeysResponse.h>
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
#include <awsmock/entity/apigateway/ApiKey.h>
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
        Dto::ApiGateway::CreateApiKeyResponse CreateApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const;

        /**
         * @brief Creates a new API key
         *
         * @param request Api gateway create key request
         * @return get API key response
         */
        [[nodiscard]]
        Dto::ApiGateway::GetApiKeysResponse GetApiKeys(const Dto::ApiGateway::GetApiKeysRequest &request) const;

        /**
         * @brief Deletes an API gateway key
         *
         * @param request delete API key request
         */
        void DeleteApiKey(const Dto::ApiGateway::DeleteApiKeyRequest &request) const;

        /**
         * @brief Creates a new REST API
         *
         * @param request create a REST API request
         * @return create REST API response
         */
        [[nodiscard]]
        Dto::ApiGateway::CreateRestApiResponse CreateRestApi(const Dto::ApiGateway::CreateRestApiRequest &request) const;

        /**
         * @brief List the API key counters
         *
         * @param request internal API key counters request
         * @return list of API key counters
         */
        [[nodiscard]]
        Dto::ApiGateway::ListApiKeyCountersResponse ListApiKeyCounters(const Dto::ApiGateway::ListApiKeyCountersRequest &request) const;

        /**
         * @brief Get an API key detail
         *
         * @param request internal API key counters request
         * @return list of API key counters
         */
        [[nodiscard]]
        Dto::ApiGateway::GetApiKeyCounterResponse GetApiKeyCounter(const Dto::ApiGateway::GetApiKeyCounterRequest &request) const;

        /**
         * @brief Updates an API key
         *
         * @param request internal API key counters update request
         * @return list of API key counters
         */
        void UpdateApiKeyCounter(const Dto::ApiGateway::UpdateApiKeyCounterRequest &request) const;

        /**
         * @brief List REST APIs
         *
         * @param request internal REST API counters request
         * @return list of REST API counters
         */
        [[nodiscard]]
        Dto::ApiGateway::ListRestApiCountersResponse ListRestApiCounters(const Dto::ApiGateway::ListRestApiCountersRequest &request) const;

        /**
         * @brief Returns a REST API entity
         *
         * @param request get a REST API request
         * @return REST API response
         */
        Dto::ApiGateway::GetRestApiCounterResponse GetRestApiCounter(const Dto::ApiGateway::GetRestApiCounterRequest &request) const;

        /**
         * @brief Updates a REST API entity
         *
         * @param request get a REST API update request
         */
        void UpdateRestApiCounter(const Dto::ApiGateway::UpdateRestApiCounterRequest &request) const;

        /**
         * @brief Deletes a REST API
         *
         * @param request delete request
         */
        void DeleteRestApiCounter(const Dto::ApiGateway::DeleteRestApiCounterRequest &request) const;

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
