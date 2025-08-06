//
// Created by vogje01 on 5/27/24.
//


#include <awsmock/service/gateway/GatewayRouter.h>

namespace AwsMock::Service {

    std::shared_ptr<AbstractHandler> GatewayRouter::GetHandler(const std::string &routingKey, tcp_stream_t &stream) {
        if (routingKey == "s3" || routingKey == "s3api") {
            return std::make_shared<S3Handler>(stream);
        }
        if (routingKey == "sqs") {
            return std::make_shared<SQSHandler>(stream);
        }
        if (routingKey == "sqs") {
            return std::make_shared<SQSHandler>(stream);
        }
        if (routingKey == "sns") {
            return std::make_shared<SNSHandler>(stream);
        }
        if (routingKey == "lambda") {
            return std::make_shared<LambdaHandler>(stream);
        }
        if (routingKey == "transfer") {
            return std::make_shared<TransferHandler>(stream);
        }
        if (routingKey == "cognito-idp" || routingKey == "cognito-identity") {
            return std::make_shared<CognitoHandler>(stream);
        }
        if (routingKey == "secretsmanager") {
            return std::make_shared<SecretsManagerHandler>(stream);
        }
        if (routingKey == "kms") {
            return std::make_shared<KMSHandler>(stream);
        }
        if (routingKey == "ssm") {
            return std::make_shared<SSMHandler>(stream);
        }
        if (routingKey == "dynamodb" || routingKey == "dynamodbstreams") {
            return std::make_shared<DynamoDbHandler>(stream);
        }
        if (routingKey == "application") {
            return std::make_shared<ApplicationHandler>(stream);
        }
        if (routingKey == "monitoring") {
            return std::make_shared<MonitoringHandler>(stream);
        }
        if (routingKey == "module") {
            return std::make_shared<ModuleHandler>(stream);
        }
        log_error << "Routing table entry not found, routingKey: " << routingKey;
        return nullptr;
    }

}// namespace AwsMock::Service
