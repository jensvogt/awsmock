//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/secretsmanager/SecretRotation.h>

namespace AwsMock::Service {

    void SecretRotation::operator()(Database::Entity::SecretsManager::Secret &secret, const std::string &clientRequestToken) const {

        log_debug << "Start rotation secret, arn: " << secret.arn;

        // Get lambda function from database
        const Database::Entity::Lambda::Lambda lambda = Database::LambdaDatabase::instance().GetLambdaByArn(secret.rotationLambdaARN);

        CreateSecret(secret, lambda, clientRequestToken);
        log_debug << "Secret created, arn: " << secret.arn;

        SetSecret(secret, lambda, clientRequestToken);
        log_debug << "Secret set in resource, arn: " << secret.arn;

        TestSecret(secret, lambda, clientRequestToken);
        log_debug << "Secret testet, arn: " << secret.arn;

        secret.nextRotatedDate = GetNextRotationDate(secret);
        secret = Database::SecretsManagerDatabase::instance().UpdateSecret(secret);
        log_debug << "Secret updated, arn: " << secret.arn;
    }

    void SecretRotation::CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(createSecret);
        std::string payload = invocationRequest.ToJson();
        SendLambdaInvocationRequest(lambda, payload);
    }

    void SecretRotation::SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(setSecret);
        std::string payload = invocationRequest.ToJson();
        SendLambdaInvocationRequest(lambda, payload);
    }

    void SecretRotation::TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(testSecret);
        std::string payload = invocationRequest.ToJson();
        SendLambdaInvocationRequest(lambda, payload);
    }

    void SecretRotation::FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(finishSecret);
        std::string payload = invocationRequest.ToJson();
        SendLambdaInvocationRequest(lambda, payload);
    }

    void SecretRotation::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, std::string &body) {
        log_debug << "Invoke lambda function request, function: " << lambda.function << " body: " << body;

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        // TODO: fix boost asio context
        //LambdaService lambdaService;
        //Dto::Lambda::LambdaResult result = lambdaService.InvokeLambdaFunction(region, lambda.function, body, Dto::Lambda::LambdaInvocationType::EVENT);
        log_debug << "Lambda send invocation request finished, function: " << lambda.function;
    }

    system_clock::time_point SecretRotation::GetNextRotationDate(const Database::Entity::SecretsManager::Secret &secret) {
        log_debug << "Get next rotation datetime, secret: " << secret.secretId;
        system_clock::time_point nextRotationDate = system_clock::now();
        if (secret.rotationRules.automaticallyAfterDays > 0) {
            nextRotationDate += std::chrono::days(secret.rotationRules.automaticallyAfterDays);
        }
        return nextRotationDate;
    }
}// namespace AwsMock::Service
