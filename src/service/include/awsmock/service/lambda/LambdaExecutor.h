//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H
#define AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H

// C++ include
#include <chrono>
#include <condition_variable>

// AwsMock includes
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/lambda/model/InvocationType.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/monitoring/MetricServiceTimer.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    using std::chrono::system_clock;

    /**
     * @brief Lambda executor.
     *
     * The executor is launched asynchronously. As the dockerized lambda runtime using AWS RIE only allows the execution of a lambda function at a time, the lambda function invocation will be
     * queued up in a Poco notification queue and executed one by one. Each invocation will wait for the finishing of the last invocation request. The lambda image can run on a remote docker
     * instance. In this case, the hostname on the invocation request has to be filled in. The default is 'localhost'.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaExecutor {

      public:

        /**
         * @brief Constructor
         */
        explicit LambdaExecutor() = default;

        /**
         * @brief Executes a lambda function
         *
         * @param lambda lambda function
         * @param containerId lambda docker container ID
         * @param host lambda docker host
         * @param port lambda docker port
         * @param payload lambda payload
         */
        static void InvocationAsync(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, int port, const std::string &payload);

        /**
         * @brief Synchronously execution a lambda function
         *
         * @param lambda lambda function
         * @param containerId lambda docker container ID
         * @param host lambda docker host
         * @param port lambda docker port
         * @param payload lambda payload
         * @param stream response stream
         * @return lambdaResult
         */
        static Database::Entity::Lambda::LambdaResult InvocationSync(const Database::Entity::Lambda::Lambda &lambda, const std::string &containerId, const std::string &host, int port, const std::string &payload, boost::beast::tcp_stream &stream);

        /**
         * @brief Executes a lambda function
         *
         * @param lambda lambda function
         * @param invocationType invocation types asynchron/synchron
         * @param containerId lambda docker container ID
         * @param host lambda docker host
         * @param port lambda docker port
         * @param payload lambda payload
         * @param _stream response stream
         */
        static void SpawnDetached(const Database::Entity::Lambda::Lambda &lambda, const Dto::Lambda::LambdaInvocationType &invocationType, const std::string &containerId, const std::string &host, int port, const std::string &payload, boost::beast::tcp_stream &_stream);

      private:

        /**
         * Metric module
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H
