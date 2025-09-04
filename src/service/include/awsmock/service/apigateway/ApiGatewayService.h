//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_API_GATEWAY_SERVICE_H
#define AWSMOCK_SERVICE_API_GATEWAY_SERVICE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/apigateway/CreateApiKeyRequest.h>
#include <awsmock/dto/apigateway/CreateApiKeyResponse.h>
#include <awsmock/dto/apigateway/mapper/Mapper.h>
#include <awsmock/entity/apigateway/Key.h>
#include <awsmock/repository/ApiGatewayDatabase.h>
#include <awsmock/repository/ApplicationDatabase.h>

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
        Dto::ApiGateway::CreateApiKeyResponse CreateApiKey(const Dto::ApiGateway::CreateApiKeyRequest &request) const;

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
