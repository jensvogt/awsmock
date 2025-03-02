//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVER_GATEWAY_ROUTER_H
#define AWSMOCK_SERVER_GATEWAY_ROUTER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/service/cognito/CognitoHandler.h>
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

namespace AwsMock::Service {

    typedef std::map<std::string, std::shared_ptr<AbstractHandler>> RoutingTable;

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
        explicit GatewayRouter();

        /**
         * @brief Singleton instance
         */
        static GatewayRouter &instance() {
            static GatewayRouter gatewayRouter;
            return gatewayRouter;
        }

        /**
         * @brief Returns an HTTP request handler for a module.
         *
         * @param routingKey module name
         * @return pointer to module handler
         */
        std::shared_ptr<AbstractHandler> GetHandler(const std::string &routingKey);

      private:

        /**
         * Routine table
         */
        RoutingTable _routingTable;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVER_GATEWAY_ROUTER_H
