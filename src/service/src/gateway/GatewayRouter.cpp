//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewayRouter.h>

namespace AwsMock::Service {

    GatewayRouter::GatewayRouter() {

        _routingTable = {
                {"s3", std::make_shared<S3Handler>()},
                {"s3api", std::make_shared<S3Handler>()},
                {"sqs", std::make_shared<SQSHandler>()},
                {"sns", std::make_shared<SNSHandler>()},
                {"lambda", std::make_shared<LambdaHandler>()},
                {"transfer", std::make_shared<TransferHandler>()},
                {"cognito-idp", std::make_shared<CognitoHandler>()},
                {"cognito-identity", std::make_shared<CognitoHandler>()},
                {"secretsmanager", std::make_shared<SecretsManagerHandler>()},
                {"kms", std::make_shared<KMSHandler>()},
                {"ssm", std::make_shared<SSMHandler>()},
                {"dynamodb", std::make_shared<DynamoDbHandler>()},
                {"monitoring", std::make_shared<MonitoringHandler>()},
                {"module", std::make_shared<ModuleHandler>()}};
    }

    std::shared_ptr<AbstractHandler> GatewayRouter::GetHandler(const std::string &routingKey) {
        if (_routingTable.contains(routingKey)) {
            return _routingTable.at(routingKey);
        }
        log_error << "Routing table entry not found, routingKey: " << routingKey;
        return nullptr;
    }

}// namespace AwsMock::Service
