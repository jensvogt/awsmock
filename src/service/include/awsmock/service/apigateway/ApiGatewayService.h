//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_API_GATEWAY_SERVICE_H
#define AWSMOCK_SERVICE_API_GATEWAY_SERVICE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include "awsmock/dto/apigateway/CreateRestApiResponse.h"


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
#include <awsmock/dto/apigateway/internal/GetApiKeyCounterRequest.h>
#include <awsmock/dto/apigateway/internal/GetApiKeyCounterResponse.h>
#include <awsmock/dto/apigateway/internal/ListApiKeyCountersRequest.h>
#include <awsmock/dto/apigateway/internal/ListApiKeyCountersResponse.h>
#include <awsmock/dto/apigateway/internal/ListRestApiCountersRequest.h>
#include <awsmock/dto/apigateway/internal/ListRestApiCountersResponse.h>
#include <awsmock/dto/apigateway/internal/UpdateApiKeyCounterRequest.h>
#include <awsmock/dto/apigateway/mapper/Mapper.h>
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/repository/ApiGatewayDatabase.h>

namespace AwsMock::Service {

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
        explicit ApiGatewayService(boost::asio::io_context &ioc) : _apiGatewayDatabase(Database::ApiGatewayDatabase::instance()), _ioc(ioc) {}

        /**
         * @brief Creates a new API key
         *
         * @param request Api gateway create key request
         * @return API gateway create key response
         */
        [[nodiscard]] Dto::ApiGateway::CreateApiKeyResponse CreateApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const;

        /**
         * @brief Creates a new API key
         *
         * @param request Api gateway create key request
         * @return get API key response
         */
        [[nodiscard]] Dto::ApiGateway::GetApiKeysResponse GetApiKeys(const Dto::ApiGateway::GetApiKeysRequest &request) const;

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
        [[nodiscard]] Dto::ApiGateway::CreateRestApiResponse CreateRestApi(const Dto::ApiGateway::CreateRestApiRequest &request) const;

        /**
         * @brief List the API key counters
         *
         * @param request internal API key counters request
         * @return list of API key counters
         */
        [[nodiscard]] Dto::ApiGateway::ListApiKeyCountersResponse ListApiKeyCounters(const Dto::ApiGateway::ListApiKeyCountersRequest &request) const;

        /**
         * @brief Get an API key detail
         *
         * @param request internal API key counters request
         * @return list of API key counters
         */
        [[nodiscard]] Dto::ApiGateway::GetApiKeyCounterResponse GetApiKeyCounter(const Dto::ApiGateway::GetApiKeyCounterRequest &request) const;

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
        [[nodiscard]] Dto::ApiGateway::ListRestApiCountersResponse ListRestApiCounters(const Dto::ApiGateway::ListRestApiCountersRequest &request) const;

      private:

        /**
         * Database connection
         */
        Database::ApiGatewayDatabase &_apiGatewayDatabase;

        /**
         * Boost asio IO context
         */
        boost::asio::io_context &_ioc;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_API_GATEWAY_SERVICE_H
