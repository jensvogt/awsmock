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
        void StartRestApis() const;

        /**
         * @brief Back up the cognito objects
         */
        static void BackupApiGateway();

        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::IApiGatewayRepository> _apiGatewayDatabase = Database::RepositoryFactory::instance().apigatewayRepository();
    };

}// namespace Awsmock::Service
