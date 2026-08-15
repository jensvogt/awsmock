//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/dto/secretsmanager/model/VersionStage.h>
#include <awsmock/repository/secretsmanager/SecretsManagerMongoRepository.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/secretsmanager/SecretRotation.h>

#define SECRETSMANAGER_DEFAULT_MONITORING_PERIOD 300
#define SECRETSMANAGER_DEFAULT_ROTATION_CHECK_PERIOD 900

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
        void shutdown() override;

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
         * @brief Checks all secrets for a due rotation and starts it, if needed.
         *
         * @par
         * Runs periodically. A secret is rotated when it has rotation enabled, a rotation lambda
         * configured, and its next rotation date has passed.
         */
        void CheckRotations() const;

        /**
         * @brief Database connection
         */
        std::shared_ptr<Database::ISecretsManagerRepository> _secretsManagerDatabase = Database::RepositoryFactory::instance().secretsmanagerRepository();
    };

} // namespace Awsmock::Service
