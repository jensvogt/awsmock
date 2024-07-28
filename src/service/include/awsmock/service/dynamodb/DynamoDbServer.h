//
// Created by vogje01 on 20/12/2023.
//

#ifndef AWSMOCK_SERVER_DYNAMODB_SERVER_H
#define AWSMOCK_SERVER_DYNAMODB_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include "awsmock/service/docker/DockerService.h"
#include <awsmock/core/LogStream.h>
#include <awsmock/core/monitoring/MetricService.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/repository/ModuleDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/dynamodb/DynamoDbMonitoring.h>
#include <awsmock/service/dynamodb/DynamoDbWorker.h>

#define DYNAMODB_DEFAULT_PORT 9507
#define DYNAMODB_DEFAULT_HOST "localhost"
#define DYNAMODB_DEFAULT_QUEUE 150
#define DYNAMODB_DEFAULT_THREADS 50
#define DYNAMODB_DEFAULT_TIMEOUT 120
#define DYNAMODB_DEFAULT_MONITORING_PERIOD 300
#define DYNAMODB_DEFAULT_WORKER_PERIOD 300
#define DYNAMODB_DOCKER_IMAGE std::string("dynamodb-local")
#define DYNAMODB_DOCKER_TAG std::string("latest")
#define DYNAMODB_INTERNAL_PORT 8000
#define DYNAMODB_EXTERNAL_PORT 8000
#define DYNAMODB_DOCKER_FILE "FROM amazon/dynamodb-local:latest\n"                           \
                             "VOLUME /home/awsmock/data/dynamodb /home/dynamodblocal/data\n" \
                             "WORKDIR /home/dynamodblocal\n"                                 \
                             "EXPOSE 8000 8000\n"                                            \
                             "ENTRYPOINT [\"java\", \"-Djava.library.path=./DynamoDBLocal_lib\", \"-jar\", \"DynamoDBLocal.jar\", \"-sharedDb\"]\n"

namespace AwsMock::Service {

    /**
     * @brief AwsMock DynamoDB server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbServer : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit DynamoDbServer();

        /**
         * @brief Initialization
         */
        void Initialize() override;

      protected:

        /**
         * @brief Main method
         */
        void Run() override;

        /**
         * @brief Shutdown
         */
        void Shutdown() override;

      private:

        /**
         * @brief Delete dangling, stopped containers
         */
        void CleanupContainers();

        /**
         * @brief Start the local DynamoDB container.
         *
         * <p>
         * If the AWS DynamoDb docker image does not already exists, it will be downloaded. Otherwise the local docker
         * image will be started as container.
         * </p>
         */
        void StartLocalDynamoDb();

        /**
         * @brief Stop the local DynamoDB container.
         *
         * <p>
         * The AWS DynamoDb docker container will be stopped.
         * </p>
         */
        void StopLocalDynamoDb();

        /**
         * Docker module
         */
        Service::DockerService &_dockerService;

        /**
         * Monitoring
         */
        std::shared_ptr<DynamoDbMonitoring> _dynamoDbMonitoring;

        /**
         * Monitoring
         */
        std::shared_ptr<DynamoDbWorker> _dynamoDbWorker;

        /**
         * AWS region
         */
        std::string _region;

        /**
         * Sleeping period in ms
         */
        int _period;

        /**
         * Rest port
         */
        int _port;

        /**
         * Rest host
         */
        std::string _host;

        /**
         * HTTP max message queue length
         */
        int _maxQueueLength;

        /**
         * HTTP max concurrent connection
         */
        int _maxThreads;

        /**
         * HTTP request timeout in seconds
         */
        int _requestTimeout;

        /**
         * Monitoring period
         */
        int _monitoringPeriod;

        /**
         * Worker period
         */
        int _workerPeriod;

        /**
         * Module name
         */
        std::string _module;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_DYNAMODB_SERVER_H
