//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H
#define AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H

// C++ include
#include <chrono>

// Boost includes
#include <boost/asio/detached.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

// AwsMock includes
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/lambda/model/LambdaResult.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/monitoring/MetricServiceTimer.h>

namespace AwsMock::Service {

    /**
     * @brief Lambda executor.
     *
     * The executor is launched asynchronously. As the dockerized lambda runtime using AWS RIE only allows the execution of a lambda function at a time, the lambda function invocation will be
     * queued up in a Poco notification queue and executed one by one. Each invocation will wait for the finishing of the last invocation request. The lambda image can run on a remote docker
     * instance. In this case, the hostname on the invocation request has to be filled in. The default is 'localhost'.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaExecutor : public boost::enable_shared_from_this<LambdaExecutor> {

      public:

        /**
         * @brief Constructor
         */
        explicit LambdaExecutor() {}

        /**
         * @brief Executes a lambda function synchronized
         *
         * @param lambda lambda function
         * @param instanceId instance ID
         * @param containerId lambda docker container ID
         * @param host lambda docker host
         * @param port lambda docker port
         * @param payload lambda payload
         */
        Database::Entity::Lambda::LambdaResult Invocation(Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId, std::string &containerId, std::string &host, int port, std::string &payload) const;

      private:

        /**
         * Metric module
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * Lambda database connection
         */
        Database::LambdaDatabase &_lambdaDatabase = Database::LambdaDatabase::instance();

        /**
         * Docker module
         */
        ContainerService &_containerService = ContainerService::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H
