//
// Created by vogje01 on 04/01/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/apigateway/ApiGatewayController.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>

namespace Awsmock::Service {

    /**
     * @brief API gateway module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit ApiGatewayServer();

        /**
         * @brief Shutdown the api-gateway server
         */
        void Shutdown() override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Starts an awsmock-agw proxy process for each registered REST API.
         */
        void StartRestApis();

        /**
         * @brief Back up the api-gateway objects
         */
        static void BackupApiGateway();

        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();

        /**
         * @brief Manages the awsmock-agw child processes (one per REST API)
         */
        ApiGatewayController _controller;
    };

}// namespace Awsmock::Service
