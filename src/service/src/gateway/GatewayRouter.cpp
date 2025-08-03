//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewayRouter.h>

namespace AwsMock::Service {

    std::shared_ptr<AbstractHandler> GatewayRouter::GetHandler(const std::string &routingKey) {
        if (routingKey == "s3" || routingKey == "s3api") {
            return std::make_shared<S3Handler>();
        }
        if (routingKey == "sqs") {
            return std::make_shared<SQSHandler>();
        }
        if (routingKey == "sqs") {
            return std::make_shared<SQSHandler>();
        }
        if (routingKey == "sns") {
            return std::make_shared<SNSHandler>();
        }
        if (routingKey == "lambda") {
            return std::make_shared<LambdaHandler>();
        }
        if (routingKey == "transfer") {
            return std::make_shared<TransferHandler>();
        }
        if (routingKey == "cognito-idp" || routingKey == "cognito-identity") {
            return std::make_shared<CognitoHandler>();
        }
        if (routingKey == "secretsmanager") {
            return std::make_shared<SecretsManagerHandler>();
        }
        if (routingKey == "kms") {
            return std::make_shared<KMSHandler>();
        }
        if (routingKey == "ssm") {
            return std::make_shared<SSMHandler>();
        }
        if (routingKey == "dynamodb" || routingKey == "dynamodbstreams") {
            return std::make_shared<DynamoDbHandler>();
        }
        if (routingKey == "application") {
            return std::make_shared<ApplicationHandler>();
        }
        if (routingKey == "monitoring") {
            return std::make_shared<MonitoringHandler>();
        }
        if (routingKey == "module") {
            return std::make_shared<ModuleHandler>();
        }
        log_error << "Routing table entry not found, routingKey: " << routingKey;
        return nullptr;
    }

}// namespace AwsMock::Service
