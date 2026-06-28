//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/lambda/LambdaMongoRepository.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/lambda/LambdaController.h>
#include <awsmock/service/module/ModuleService.h>

namespace Awsmock::Service {

    /**
     * @brief Lambda server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaServer final : public AbstractServer {

    public:
        /**
         * @brief Constructor
         */
        explicit LambdaServer();

        /**
         * @brief Shutdown server
         */
        void shutdown() override;

        /**
         * @brief Initialization
         */
        void Initialize();

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Lambda"};

        /**
         * @brief Delete dangling, stopped containers
         */
        void CleanupDocker() const;

        /**
         * @brief Delete instances from the database, which are not running
         *
         * @par
         * This will stop and delete the lambda containers. This is done in case the shutdown was not gracefully and the lambdas are in
         * an invalid state. Especially the port is not valid anymore. Deleting the lambda function will recreate the lambda later on with
         * the correct port.
         */
        void CleanupInstances() const;

        /**
         * @brief Creates a local network.
         *
         * @par
         * The lambda functions need to connect to a local bridged network, otherwise they cannot communicate with the awsmock manager.
         */
        void CreateLocalNetwork() const;

        /**
         * @brief Creates the lambda docker/podman containers
         *
         * @par
         * For each lambda function, the container will be created, if not existing already
         */
        void CreateContainers() const;

        /**
         * @brief Remove expired lambda functions
         *
         * @par
         * Loops over all lambda functions and removes the lambda container when the lambdas are expired.
         */
        void RemoveExpiredLambdas() const;

        /**
         * @brief Remove expired lambda logs
         *
         * @par
         * The log retention period is defined in the configuration file.
         */
        void RemoveExpiredLambdaLogs() const;

        /**
         * Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Back up the Lambda objects
         */
        static void BackupLambda();

        /**
         * Lambda database
         */
        std::shared_ptr<Database::ILambdaRepository> _lambdaDatabase = Database::RepositoryFactory::instance().lambdaRepository();

        /**
         * Docker module
         */
        ContainerService _dockerService;

        /**
         * Lambda controller â€” reacts to EventBus lambda signals
         */
        LambdaController _lambdaController;

        /**
         * @brief Dynamo DB backup flag.
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        bool _backupActive{};

        /**
         * @brief Dynamo DB backup cron schedule.
         *
         * @par
         * Cron schedule in form '* * * * * ?', with seconds, minutes, hours, dayOfMonth, month, dayOfWeek, year (optional)
         *
         * @see @link(https://github.com/mariusbancila/croncpp)croncpp
         */
        std::string _backupCron;

        /**
         * Data dir
         */
        std::string _lambdaDir;

        /**
         * AWS region
         */
        std::string _region;

        /**
         * Counter period in seconds
         */
        int _counterPeriod{};

        /**
         * Remove period in seconds
         */
        int _lifetime{};

        /**
         * Lifetime check interval
         */
        int _removePeriod{};

        /**
         * Log retention period in days
         */
        int _logRetentionPeriod{};
    };

} // namespace Awsmock::Service
