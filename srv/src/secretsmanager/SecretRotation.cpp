//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/secretsmanager/SecretRotation.h>

namespace Awsmock::Service {

    void SecretRotation::operator()(Database::Entity::SecretsManager::Secret &secret, const std::string &clientRequestToken) const {

        log_debug << "Start rotation secret, arn: " << secret.arn;

        // This runs detached from the request thread (see SecretsManagerService::RotateSecret), so
        // an uncaught exception here would call std::terminate and take the whole process down.
        try {

            // Get lambda function from database
            const Database::Entity::Lambda::Lambda lambda = Database::RepositoryFactory::instance().lambdaRepository()->getLambdaByArn(secret.rotationLambdaARN);

            CreateSecret(secret, lambda, clientRequestToken);
            log_debug << "Secret created, arn: " << secret.arn;

            SetSecret(secret, lambda, clientRequestToken);
            log_debug << "Secret set in resource, arn: " << secret.arn;

            TestSecret(secret, lambda, clientRequestToken);
            log_debug << "Secret testet, arn: " << secret.arn;

            FinishSecret(secret, lambda, clientRequestToken);
            log_debug << "Secret rotation finished, arn: " << secret.arn;

            // Re-fetch before this final write: `secret` is the pre-rotation snapshot captured
            // when this detached thread was started, and UpdateSecret() replaces the whole
            // document. Writing it back as-is would silently revert everything the 4 lambda
            // steps just did (new secret value, promoted AWSCURRENT/AWSPREVIOUS) to their
            // pre-rotation state.
            secret = Database::RepositoryFactory::instance().secretsmanagerRepository()->GetSecretByArn(secret.arn);
            secret.nextRotatedDate = GetNextRotationDate(secret);
            secret = Database::RepositoryFactory::instance().secretsmanagerRepository()->UpdateSecret(secret);
            log_debug << "Secret updated, arn: " << secret.arn;

        } catch (std::exception &exc) {
            log_error << "Secret rotation failed, arn: " << secret.arn << ", error: " << exc.what();
        }
    }

    void SecretRotation::CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

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

    void SecretRotation::SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

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

    void SecretRotation::TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

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

    void SecretRotation::FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken) const {

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

    void SecretRotation::SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, std::string &body) const {
        log_debug << "Invoke lambda function request, function: " << lambda.function << " body: " << body;

        const auto region = Core::Configuration::instance().get<std::string>("awsmock.region");
        const LambdaService lambdaService;
        const Dto::Lambda::LambdaResult result = lambdaService.InvokeLambdaFunction(region, lambda.function, body, Dto::Lambda::LambdaInvocationType::REQUEST_RESPONSE);
        if (result.status < 200 || result.status >= 300) {
            log_error << "Lambda invocation failed, function: " << lambda.function << ", status: " << result.status << ", body: " << result.responseBody;
            throw Core::ServiceException("Lambda invocation failed, function: " + lambda.function + ", status: " + std::to_string(result.status) + ", body: " + result.responseBody);
        }

        // A Lambda function that raises an unhandled exception still gets HTTP 200 back from the
        // runtime (matching real AWS Lambda, which signals this via the invoke response rather
        // than the HTTP status) - the awsmock-lrt runtime reports it as {"error": "..."} in the
        // body instead of the X-Amz-Function-Error header. Without checking this, a failed
        // rotation step would be treated as successful and the chain would march on regardless.
        try {
            const boost::json::value responseValue = boost::json::parse(result.responseBody);
            if (responseValue.is_object() && responseValue.as_object().contains("error")) {
                const std::string errorMessage = boost::json::serialize(responseValue.as_object().at("error"));
                log_error << "Lambda function reported an error, function: " << lambda.function << ", error: " << errorMessage;
                throw Core::ServiceException("Lambda function reported an error, function: " + lambda.function + ", error: " + errorMessage);
            }
        } catch (const boost::system::system_error &) {
            // Response body is not JSON - not an error-shaped response, nothing to do.
        }

        log_debug << "Lambda send invocation request finished, function: " << lambda.function << ", status: " << result.status;
    }

    system_clock::time_point SecretRotation::GetNextRotationDate(const Database::Entity::SecretsManager::Secret &secret) const {
        log_debug << "Get next rotation datetime, secret: " << secret.secretId;
        system_clock::time_point nextRotationDate = system_clock::now();
        if (secret.rotationRules.automaticallyAfterDays > 0) {
            nextRotationDate += std::chrono::days(secret.rotationRules.automaticallyAfterDays);
        }
        return nextRotationDate;
    }
}// namespace Awsmock::Service
