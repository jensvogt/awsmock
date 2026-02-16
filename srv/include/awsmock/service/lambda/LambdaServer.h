//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_SERVER_H
#define AWSMOCK_SERVICE_LAMBDA_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/dto/container/CreateNetworkRequest.h>
#include <awsmock/dto/lambda/mapper/Mapper.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/lambda/LambdaCreator.h>
#include <awsmock/service/lambda/LambdaExecutor.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {

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
        explicit LambdaServer(Core::Scheduler &scheduler, boost::asio::io_context &ioc);

        /**
         * @brief Shutdown server
         */
        void Shutdown() override;

      private:

        /**
         * @brief Delete dangling, stopped containers
         */
        void CleanupDocker() const;

        /**
         * @brief Delete instances from database, which are not running
         *
         * @par
         * This will stop and delete the lambda containers. This is done in case the shutdown was not gracefully and the lambdas are in
         * an invalid state. Specially the port is not valid anymore. Deleting the lambda function will recreate the lambda later on with
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
         * @brief Backup the Lambda objects
         */
        static void BackupLambda();

        /**
         * Lambda database
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * Docker module
         */
        ContainerService _dockerService;

        /**
         * Lambda service module
         */
        LambdaService _lambdaService;

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * @brief Dynamo DB backup flag.
         *
         * @par
         * If true, backup tables and items based on cron expression
         */
        bool _backupActive;

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
        int _counterPeriod;

        /**
         * Remove period in seconds
         */
        int _lifetime;

        /**
         * Log retention period in days
         */
        int _logRetentionPeriod;

        /**
         * Monitoring collector
         */
        Core::MonitoringCollector &_monitoringCollector;

        /**
         * Asynchronous task scheduler
         */
        Core::Scheduler &_scheduler;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_SERVER_H
