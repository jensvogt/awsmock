//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H
#define AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H

// AwsMock includes
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/lambda/model/LambdaResult.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricService.h>

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
        explicit LambdaExecutor() : _monitoringCollector(Core::MonitoringCollector::instance()) {}

        /**
         * @brief Executes a lambda function synchronized
         *
         * @param lambda lambda function
         * @param instance lambda instance ID
         * @param payload lambda payload
         */
        Database::Entity::Lambda::LambdaResult Invocation(Database::Entity::Lambda::Lambda &lambda, Database::Entity::Lambda::Instance &instance, std::string &payload) const;

      private:

        /**
         * Monitoring collector
         */
        Core::MonitoringCollector &_monitoringCollector;

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
