//
// Created by vogje01 on 20/12/2023.
//

#ifndef AWSMOCK_SERVER_DYNAMODB_SERVER_H
#define AWSMOCK_SERVER_DYNAMODB_SERVER_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BackupUtils.h>
#include <awsmock/core/EventBus.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/dto/dynamodb/DescribeTableResponse.h>
#include <awsmock/dto/dynamodb/ListTableResponse.h>
#include <awsmock/dto/module/ExportInfrastructureRequest.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>
#include <awsmock/service/module/ModuleService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief AwsMock DynamoDB server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit DynamoDbServer(Core::Scheduler &scheduler);

        /**
         * @brief Gracefully shutdown the server
         */
        void Shutdown() override;

      private:

        /**
         * @brief Creates a local network.
         *
         * @par
         * The lambda functions need to connect to a local bridged network, otherwise they cannot communicate with the awsmock manager.
         */
        void CreateLocalNetwork() const;

        /**
         * @brief Start the local DynamoDB container.
         *
         * @par
         * If the AWS DynamoDb docker image does not already exist, it will be downloaded. Otherwise, the local docker
         * image will be started as a container.
         */
        void StartLocalDynamoDb() const;

        /**
         * @brief Stop the local DynamoDB container.
         *
         * @par
         * The AWS DynamoDb docker container will be stopped.
         */
        void StopLocalDynamoDb() const;

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Synchronize tables.
         *
         * @par
         * Loops over all DynamoDB tables and updates the MongoDB backend.
         */
        void SynchronizeTables() const;

        /**
         * @brief Synchronize items.
         *
         * @par
         * Loops over all DynamoDB table items and updates the MongoDB backend.
         */
        void SynchronizeItems() const;

        /**
         * @brief Backup the dynamoDb tables and items
         */
        static void BackupDynamoDb();

        /**
         * @brief Writes the docker file
         */
        static std::string WriteDockerFile();

        /**
         * Container module (either docker or podman)
         */
        ContainerService &_containerService;

        /**
         * DynamoDB service
         */
        DynamoDbService _dynamoDbService;

        /**
         * Database connection
         */
        Database::DynamoDbDatabase &_dynamoDbDatabase;

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService;

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
         * Monitoring period
         */
        int _monitoringPeriod;

        /**
         * Worker period
         */
        int _workerPeriod;

        /**
         * Dynamo DB docker name
         */
        std::string _containerName;

        /**
         * Dynamo DB docker host
         */
        std::string _containerHost;

        /**
         * Dynamo DB docker host
         */
        int _containerPort;

        /**
         * Dynamo DB image name
         */
        std::string _imageName;

        /**
         * Dynamo DB image tag
         */
        std::string _imageTag;

        /**
         * AWS region
         */
        std::string _region;

        /**
         * Data directory
         */
        std::string _dataDir;

        /**
         * Asynchronous task scheduler
         */
        Core::Scheduler &_scheduler;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_DYNAMODB_SERVER_H
