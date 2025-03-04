//
// Created by vogje01 on 20/12/2023.
//

#ifndef AWSMOCK_SERVER_DYNAMODB_SERVER_H
#define AWSMOCK_SERVER_DYNAMODB_SERVER_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/dto/dynamodb/DescribeTableResponse.h>
#include <awsmock/dto/dynamodb/ListTableResponse.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>

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
    class DynamoDbServer final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit DynamoDbServer(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Start the local DynamoDB container.
         *
         * <p>
         * If the AWS DynamoDb docker image does not already exists, it will be downloaded. Otherwise the local docker
         * image will be started as container.
         * </p>
         */
        void StartLocalDynamoDb() const;

        /**
         * @brief Stop the local DynamoDB container.
         *
         * <p>
         * The AWS DynamoDb docker container will be stopped.
         * </p>
         */
        void StopLocalDynamoDb() const;

        /**
         * @brief Update counters
         */
        void UpdateCounter() const;

        /**
         * @brief Synchronize tables.
         *
         * Loops over all DynamoDB tables an updates the MongoDB backend.
         */
        void SynchronizeTables() const;

        /**
         * @brief Synchronize items.
         *
         * Loops over all DynamoDB table items an updates the MongoDB backend.
         */
        void SynchronizeItems() const;

        /**
         * Docker module
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
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_DYNAMODB_SERVER_H
