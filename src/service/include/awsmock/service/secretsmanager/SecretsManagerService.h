//
// Created by vogje01 on 07/04/2024.
//

#ifndef AWSMOCK_SERVICE_SECRETSMANAGER_SERVICE_H
#define AWSMOCK_SERVICE_SECRETSMANAGER_SERVICE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/secretsmanager/CreateSecretRequest.h>
#include <awsmock/dto/secretsmanager/CreateSecretResponse.h>
#include <awsmock/dto/secretsmanager/DeleteSecretRequest.h>
#include <awsmock/dto/secretsmanager/DeleteSecretResponse.h>
#include <awsmock/dto/secretsmanager/DescribeSecretRequest.h>
#include <awsmock/dto/secretsmanager/DescribeSecretResponse.h>
#include <awsmock/dto/secretsmanager/GetSecretValueRequest.h>
#include <awsmock/dto/secretsmanager/GetSecretValueResponse.h>
#include <awsmock/dto/secretsmanager/ListSecretsRequest.h>
#include <awsmock/dto/secretsmanager/ListSecretsResponse.h>
#include <awsmock/dto/secretsmanager/RotateSecretRequest.h>
#include <awsmock/dto/secretsmanager/RotateSecretResponse.h>
#include <awsmock/dto/secretsmanager/UpdateSecretRequest.h>
#include <awsmock/dto/secretsmanager/UpdateSecretResponse.h>
#include <awsmock/repository/SecretsManagerDatabase.h>

namespace AwsMock::Service {

    /**
     * @brief Secrets manager service.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerService {

      public:

        /**
         * Constructor
         */
        explicit SecretsManagerService();

        /**
         * Create a new secret
         *
         * @param request create secret request
         * @return CreateSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::CreateSecretResponse CreateSecret(const Dto::SecretsManager::CreateSecretRequest &request) const;

        /**
         * Describes an existing secret
         *
         * @param request describe secret request
         * @return DescribeSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::DescribeSecretResponse DescribeSecret(const Dto::SecretsManager::DescribeSecretRequest &request) const;

        /**
         * Returns the secret value
         *
         * @param request get secret value request
         * @return GetSecretValueResponse
         */
        [[nodiscard]] Dto::SecretsManager::GetSecretValueResponse GetSecretValue(const Dto::SecretsManager::GetSecretValueRequest &request) const;

        /**
         * List existing secrets
         *
         * @param request list secrets request
         * @return ListSecretsResponse
         */
        [[nodiscard]] Dto::SecretsManager::ListSecretsResponse ListSecrets(const Dto::SecretsManager::ListSecretsRequest &request) const;

        /**
         * Updates an existing secret
         *
         * @param request update secret request
         * @return UpdateSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::UpdateSecretResponse UpdateSecret(const Dto::SecretsManager::UpdateSecretRequest &request) const;

        /**
         * Rotates an existing secret
         *
         * @param request rotate secret request
         * @return RotateSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::RotateSecretResponse RotateSecret(const Dto::SecretsManager::RotateSecretRequest &request) const;

        /**
         * Deletes an existing secret
         *
         * @param request delete secret request
         * @return DeleteSecretResponse
         */
        [[nodiscard]] Dto::SecretsManager::DeleteSecretResponse DeleteSecret(const Dto::SecretsManager::DeleteSecretRequest &request) const;

      private:

        /**
         * Account ID
         */
        std::string _accountId;

        /**
         * Database connection
         */
        Database::SecretsManagerDatabase &_database;

        /**
         * Shutdown mutex
         */
        boost::mutex _mutex;

        /**
         * Key management system key
         */
        std::string _kmsKey;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SECRETSMANAGER_SERVICE_H
