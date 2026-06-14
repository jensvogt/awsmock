//
// Created by vogje01 on 03/06/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/service/apigateway/ApiGatewayHandler.h>
#include <awsmock/service/application/ApplicationHandler.h>
#include <awsmock/service/cognito/CognitoHandler.h>
#include <awsmock/service/container/ContainerHandler.h>
#include <awsmock/service/dynamodb/DynamoDbHandler.h>
#include <awsmock/service/kms/KMSHandler.h>
#include <awsmock/service/lambda/LambdaHandler.h>
#include <awsmock/service/module/ModuleHandler.h>
#include <awsmock/service/monitoring/MonitoringHandler.h>
#include <awsmock/service/s3/S3Handler.h>
#include <awsmock/service/secretsmanager/SecretsManagerHandler.h>
#include <awsmock/service/sns/SNSHandler.h>
#include <awsmock/service/sqs/SQSHandler.h>
#include <awsmock/service/ssm/SSMHandler.h>
#include <awsmock/service/transfer/TransferHandler.h>

namespace Awsmock::Service {

    /**
     * @brief Gateway router
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class GatewayRouter final {

      public:

        /**
         * @brief Constructor
         */
        explicit GatewayRouter() = default;

        /**
         * @brief Returns an HTTP request handler for a module.
         *
         * @param ioc boost asio IO context
         * @param routingKey module name
         * @return pointer to module handler
         */
        std::shared_ptr<AbstractHandler> GetHandler(const std::string &routingKey, boost::asio::io_context &ioc);

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Gateway"};
    };

}// namespace Awsmock::Service
