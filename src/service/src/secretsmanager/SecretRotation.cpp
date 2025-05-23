//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/secretsmanager/SecretRotation.h>

namespace AwsMock::Service {

    void SecretRotation::operator()(const Database::Entity::SecretsManager::Secret &secret, const std::string &clientRequestToken) const {

        log_debug << "Start rotation secret, arn: " << secret.arn;

        // Get lambda function from database
        const Database::Entity::Lambda::Lambda lambda = _lambdaDatabase.GetLambdaByArn(secret.rotationLambdaARN);

        CreateSecret(secret, lambda, clientRequestToken);
        log_debug << "Secret created, arn: " << secret.arn;

        SetSecret(secret, lambda, clientRequestToken);
        log_debug << "Secret set in resource, arn: " << secret.arn;

        TestSecret(secret, lambda, clientRequestToken);
        log_debug << "Secret testet, arn: " << secret.arn;
    }

    void SecretRotation::CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(createSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretRotation::SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(createSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretRotation::TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(testSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretRotation::FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

        // Sent create request to lambda function
        Dto::SecretsManager::LambdaInvocationRequest invocationRequest;
        invocationRequest.secretId = secret.secretId;
        invocationRequest.region = secret.region;
        invocationRequest.clientRequestToken = clientRequestToken;
        invocationRequest.requestId = clientRequestToken;
        invocationRequest.step = TaskTypeToString(testSecret);
        SendLambdaInvocationRequest(lambda, invocationRequest.ToJson());
    }

    void SecretRotation::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const std::string &body) const {
        log_debug << "Invoke lambda function request, function: " << lambda.function << " body: " << body;

        const auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        _lambdaService.InvokeLambdaFunction(region, lambda.function, body, {});
        log_debug << "Lambda send invocation request finished, function: " << lambda.function;
    }
}// namespace AwsMock::Service
