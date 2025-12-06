//
// Created by vogje01 on 07/04/2024.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_SERVICE_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_SERVICE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/secretsmanager/internal/UpdateSecretDetailsRequest.h"
#include "awsmock/dto/secretsmanager/internal/UpdateSecretDetailsResponse.h"


#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/secretsmanager/CreateSecretRequest.h>
#include <awsmock/dto/secretsmanager/CreateSecretResponse.h>
#include <awsmock/dto/secretsmanager/DeleteSecretRequest.h>
#include <awsmock/dto/secretsmanager/DeleteSecretResponse.h>
#include <awsmock/dto/secretsmanager/DescribeSecretRequest.h>
#include <awsmock/dto/secretsmanager/DescribeSecretResponse.h>
#include <awsmock/dto/secretsmanager/GetSecretValueRequest.h>
#include <awsmock/dto/secretsmanager/GetSecretValueResponse.h>
#include <awsmock/dto/secretsmanager/LambdaInvocationRequest.h>
#include <awsmock/dto/secretsmanager/ListSecretVersionIdsRequest.h>
#include <awsmock/dto/secretsmanager/ListSecretVersionIdsResponse.h>
#include <awsmock/dto/secretsmanager/ListSecretsRequest.h>
#include <awsmock/dto/secretsmanager/ListSecretsResponse.h>
#include <awsmock/dto/secretsmanager/PutSecretValueRequest.h>
#include <awsmock/dto/secretsmanager/PutSecretValueResponse.h>
#include <awsmock/dto/secretsmanager/RotateSecretRequest.h>
#include <awsmock/dto/secretsmanager/RotateSecretResponse.h>
#include <awsmock/dto/secretsmanager/UpdateSecretRequest.h>
#include <awsmock/dto/secretsmanager/UpdateSecretResponse.h>
#include <awsmock/dto/secretsmanager/internal/GetSecretDetailsRequest.h>
#include <awsmock/dto/secretsmanager/internal/GetSecretDetailsResponse.h>
#include <awsmock/dto/secretsmanager/internal/ListSecretCountersRequest.h>
#include <awsmock/dto/secretsmanager/internal/ListSecretCountersResponse.h>
#include <awsmock/dto/secretsmanager/internal/ListSecretVersionCountersRequest.h>
#include <awsmock/dto/secretsmanager/internal/ListSecretVersionCountersResponse.h>
#include <awsmock/dto/secretsmanager/internal/UpdateSecretDetailsRequest.h>
#include <awsmock/dto/secretsmanager/internal/UpdateSecretDetailsResponse.h>
#include <awsmock/dto/secretsmanager/mapper/Mapper.h>
#include <awsmock/dto/secretsmanager/model/VersionStage.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/repository/SecretsManagerDatabase.h>
#include <awsmock/service/kms/KMSService.h>
#include <awsmock/service/lambda/LambdaService.h>
#include <awsmock/service/secretsmanager/SecretRotation.h>

namespace AwsMock::Service {

    /**
     * @brief Secrets manager service.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerService {

      public:

        /**
         * @brief Constructor
         */
        explicit SecretsManagerService(boost::asio::io_context &ioc);

        /**
         * @brief Create a new secret
         *
         * @param request create a secret request
         * @return CreateSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::CreateSecretResponse CreateSecret(const Dto::SecretsManager::CreateSecretRequest &request) const;

        /**
         * @brief Describes an existing secret
         *
         * @param request describe secret request
         * @return DescribeSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::DescribeSecretResponse DescribeSecret(const Dto::SecretsManager::DescribeSecretRequest &request) const;

        /**
         * @brief Returns the secret value
         *
         * @param request get secret value request
         * @return GetSecretValueResponse
         */
        [[nodiscard]] Dto::SecretsManager::GetSecretValueResponse GetSecretValue(const Dto::SecretsManager::GetSecretValueRequest &request) const;

        /**
         * @brief Puts a secret value
         *
         * @param request put secret value request
         * @return PutSecretValueResponse
         */
        [[nodiscard]] Dto::SecretsManager::PutSecretValueResponse PutSecretValue(const Dto::SecretsManager::PutSecretValueRequest &request) const;

        /**
         * @brief List existing secrets
         *
         * @param request list secrets request
         * @return ListSecretsResponse
         */
        [[nodiscard]] Dto::SecretsManager::ListSecretsResponse ListSecrets(const Dto::SecretsManager::ListSecretsRequest &request) const;

        /**
         * @brief List secret version IDs
         *
         * @param request list secret versions request
         * @return ListSecretVersionIdsResponse
         */
        [[nodiscard]] Dto::SecretsManager::ListSecretVersionIdsResponse ListSecretVersionIds(const Dto::SecretsManager::ListSecretVersionIdsRequest &request) const;

        /**
         * @brief List secret counters
         *
         * @param request list secret counters request
         * @return ListSecretCountersResponse
         */
        [[nodiscard]] Dto::SecretsManager::ListSecretCountersResponse ListSecretCounters(const Dto::SecretsManager::ListSecretCountersRequest &request) const;

        /**
         * @brief List secret versions
         *
         * @param request list secret version counters request
         * @return ListSecretVersionCountersResponse
         */
        [[nodiscard]] Dto::SecretsManager::ListSecretVersionCountersResponse ListSecretVersionCounters(const Dto::SecretsManager::ListSecretVersionCountersRequest &request) const;

        /**
         * @brief Returns the secret details
         *
         * @param request get secret details request
         * @return GetSecretDetailsResponse
         */
        [[nodiscard]] Dto::SecretsManager::GetSecretDetailsResponse GetSecretDetails(const Dto::SecretsManager::GetSecretDetailsRequest &request) const;

        /**
         * @brief Updates an existing secret
         *
         * @param request update secret request
         * @return UpdateSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::UpdateSecretResponse UpdateSecret(const Dto::SecretsManager::UpdateSecretRequest &request) const;

        /**
         * @brief Updates an existing secret
         *
         * @param request update secret request
         * @return UpdateSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::UpdateSecretDetailsResponse UpdateSecretDetails(const Dto::SecretsManager::UpdateSecretDetailsRequest &request) const;

        /**
         * @brief Rotates an existing secret
         *
         * @param request rotate secret request
         * @return RotateSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::RotateSecretResponse RotateSecret(const Dto::SecretsManager::RotateSecretRequest &request) const;

        /**
         * @brief Deletes an existing secret
         *
         * @param request delete secret request
         * @return DeleteSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::DeleteSecretResponse DeleteSecret(const Dto::SecretsManager::DeleteSecretRequest &request) const;

      private:

        /**
         * @brief Create a secret
         *
         * @param secret to create
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void CreateSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Set a secret in the resource
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void SetSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Test the new secret
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void TestSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Finish secret rotation
         *
         * @param secret to set
         * @param lambda lambda function to invoke
         * @param clientRequestToken client request token
         */
        void FinishSecret(const Database::Entity::SecretsManager::Secret &secret, const Database::Entity::Lambda::Lambda &lambda, const std::string &clientRequestToken);

        /**
         * @brief Send a lambda invocation request
         *
         * @param lambda lambda entity
         * @param body message body
         */
        void SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const std::string &body);

        /**
         * @brief Create a new KMS key for the secret
         *
         * @param secret to create KMS key for
         */
        void CreateKmsKey(Database::Entity::SecretsManager::Secret &secret) const;

        /**
         * @brief Encrypt the secret string using the given KMS key.
         *
         * @param version secret version
         * @param kmsKeyId KMS key ID
         * @param secretString secret string
         */
        void EncryptSecret(Database::Entity::SecretsManager::SecretVersion &version, const std::string &kmsKeyId, const std::string &secretString) const;

        /**
         * @brief Decrypt the secret string using the given KMS key.
         *
         * @param version secret version
         * @param kmsKeyId KMS key ID
         * @param secretString secret string
         * @return secret string
         */
        std::string DecryptSecret(Database::Entity::SecretsManager::SecretVersion &version, const std::string &kmsKeyId, const std::string &secretString) const;

        /**
         * @brief Returns the decrypted raw secret string
         *
         * @param secret secret
         */
        std::string GetSecretString(Database::Entity::SecretsManager::Secret &secret) const;

        /**
         * Account ID
         */
        std::string _accountId;

        /**
         * Secrets manager database connection
         */
        Database::SecretsManagerDatabase &_secretsManagerDatabase;

        /**
         * Lambda database connection
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * Lambda service
         */
        LambdaService _lambdaService;

        /**
         * KMS service
         */
        KMSService _kmsService;

        /**
         * Shutdown mutex
         */
        boost::mutex _mutex;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_SERVICE_H
