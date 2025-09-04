//
// Created by vogje01 on 5/27/24.
//

#include <awsmock/service/gateway/GatewayRouter.h>

namespace AwsMock::Service {

    std::shared_ptr<AbstractHandler> GatewayRouter::GetHandler(const std::string &routingKey, boost::asio::io_context &ioc) {
        if (routingKey == "s3" || routingKey == "s3api") {
            return std::make_shared<S3Handler>(ioc);
        }
        if (routingKey == "sqs") {
            return std::make_shared<SQSHandler>(ioc);
        }
        if (routingKey == "sns") {
            return std::make_shared<SNSHandler>(ioc);
        }
        if (routingKey == "lambda") {
            return std::make_shared<LambdaHandler>(ioc);
        }
        if (routingKey == "transfer") {
            return std::make_shared<TransferHandler>(ioc);
        }
        if (routingKey == "cognito-idp" || routingKey == "cognito-identity") {
            return std::make_shared<CognitoHandler>(ioc);
        }
        if (routingKey == "secretsmanager") {
            return std::make_shared<SecretsManagerHandler>(ioc);
        }
        if (routingKey == "kms") {
            return std::make_shared<KMSHandler>(ioc);
        }
        if (routingKey == "ssm") {
            return std::make_shared<SSMHandler>(ioc);
        }
        if (routingKey == "dynamodb" || routingKey == "dynamodbs") {
            return std::make_shared<DynamoDbHandler>(ioc);
        }
        if (routingKey == "application") {
            return std::make_shared<ApplicationHandler>(ioc);
        }
        if (routingKey == "apigateway") {
            return std::make_shared<ApiGatewayHandler>(ioc);
        }
        if (routingKey == "monitoring") {
            return std::make_shared<MonitoringHandler>(ioc);
        }
        if (routingKey == "module") {
            return std::make_shared<ModuleHandler>(ioc);
        }
        log_error << "Routing table entry not found, routingKey: " << routingKey;
        return nullptr;
    }

}// namespace AwsMock::Service
