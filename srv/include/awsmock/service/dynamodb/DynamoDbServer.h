//
// Created by vogje01 on 20/12/2023.
//

#pragma once

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
#include <awsmock/repository/dynamodb/IDynamoDbRepository.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>
#include <awsmock/service/module/ModuleService.h>

namespace Awsmock::Service {

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
        explicit DynamoDbServer();

        /**
         * @brief Gracefully shutdown the server
         */
        void Shutdown() override;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "DynamoDB"};

        /**
         * @brief Creates a local network.
         *
         * @par
         * The lambda functions need to connect to a local bridged network, otherwise they cannot communicate with the awsmock manager.
         */
        void CreateLocalNetwork() const;

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Back up the dynamoDb tables and items
         */
        static void BackupDynamoDb();

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
        std::shared_ptr<Database::IDynamoDbRepository> _dynamoDbDatabase = Database::RepositoryFactory::instance().dynamodbRepository();

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
         * AWS region
         */
        std::string _region;

        /**
         * Data directory
         */
        std::string _dataDir;
    };

}// namespace Awsmock::Service
