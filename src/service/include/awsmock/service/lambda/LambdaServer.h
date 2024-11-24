//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_SERVER_H
#define AWSMOCK_SERVICE_LAMBDA_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/dto/docker/CreateNetworkRequest.h>
#include <awsmock/dto/lambda/mapper/Mapper.h>
#include <awsmock/dto/lambda/model/InvocationNotification.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/docker/DockerService.h>
#include <awsmock/service/lambda/LambdaCreator.h>
#include <awsmock/service/lambda/LambdaExecutor.h>
#include <awsmock/service/s3/S3Service.h>

#define LAMBDA_DEFAULT_MONITORING_PERIOD 300
#define LAMBDA_DEFAULT_WORKER_PERIOD 300
#define LAMBDA_DEFAULT_LIFETIME 3600

namespace AwsMock::Service {

    /**
     * @brief Lambda server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaServer : public AbstractServer {

      public:

        /**
         * Constructor
         */
        explicit LambdaServer(Core::PeriodicScheduler &scheduler);

        /**
         * Shutdown server
         */
        void Shutdown() override;

      private:

        /**
         * @brief Delete dangling, stopped containers
         */
        void CleanupContainers();

        /**
         * @brief Delete instances from database, which are not running
         */
        void CleanupInstances();

        /**
         * @brief Creates a local network.
         *
         * @par
         * The lambda functions need to connect to a local bridged network, otherwise they cannot communicate with the awsmock manager.
         */
        void CreateLocalNetwork();

        /**
         * @brief Remove expired lambda functions
         *
         * @par
         * Loops over all lambda functions and removes the lambda container, when the lambdas are expired.
         */
        void RemoveExpiredLambdas();

        /**
         * Update counters
         */
        void UpdateCounter();

        /**
         * lambda database
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * Docker module
         */
        Service::DockerService _dockerService;

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * Data dir
         */
        std::string _lambdaDir;

        /**
         * AWS region
         */
        std::string _region;

        /**
         * Monitoring period
         */
        int _monitoringPeriod;

        /**
         * Counter period
         */
        int _counterPeriod;

        /**
         * Remove period
         */
        int _removePeriod;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_SERVER_H
