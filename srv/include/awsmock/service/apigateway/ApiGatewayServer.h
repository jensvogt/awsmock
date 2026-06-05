//
// Created by vogje01 on 04/01/2023.
//

#pragma once

// C++ includes
#include <list>
#include <thread>
#include <vector>

// Boost includes
#include <boost/asio/io_context.hpp>

// AwsMock includes
#include <awsmock/agw/ProxyListener.h>
#include <awsmock/core/EventBus.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/dto/apigateway/model/ProxyConfig.h>
#include <awsmock/repository/RepositoryFactory.h>
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
         * @brief Shutdown the cognito server
         */
        void Shutdown() override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Cognito"};

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;


        /**
         * @brief Initiates and starts the REST APIs for the application.
         *
         * This method is responsible for activating and launching the REST API endpoints
         * within the application, ensuring they are ready to handle incoming requests.
         */
        void StartRestApis();

        /**
         * @brief Back up the cognito objects
         */
        static void BackupApiGateway();

        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();

        /**
         * @brief io_context shared by all in-process ProxyListeners
         */
        boost::asio::io_context _ioc;

        /**
         * @brief Stable storage for ProxyConfig objects (ProxyListener holds references)
         */
        std::list<Dto::ApiGateway::ProxyConfig> _proxyConfigs;

        /**
         * @brief Thread pool driving _ioc
         */
        std::vector<std::thread> _ioThreads;
    };

}// namespace Awsmock::Service
