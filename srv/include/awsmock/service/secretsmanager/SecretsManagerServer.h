//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <../../../../../db/include/awsmock/repository/secretsmanager/SecretsManagerMongoRepository.h>
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>

#define SECRETSMANAGER_DEFAULT_MONITORING_PERIOD 300

namespace Awsmock::Service {

    /**
     * @brief Secret manager module server.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit SecretsManagerServer();

        /**
         * @brief Shutdown server
         */
        void Shutdown() override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SecretsManager"};

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Back up the secrets manager objects
         */
        static void BackupSecretsManger();

        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::ISecretsManagerRepository> _secretsManagerDatabase = Database::RepositoryFactory::instance().secretsmanagerRepository();
    };

}// namespace Awsmock::Service
