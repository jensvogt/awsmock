//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/secretsmanager/Secret.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/secretsmanager/ISecretsManagerRepository.h>

namespace Awsmock::Database {

    /**
     * @brief Secrets manager MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerMongoRepository final : public ISecretsManagerRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit SecretsManagerMongoRepository() = default;

        /**
         * @brief Secret exists
         *
         * @param region AWS region
         * @param name secret name
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool SecretExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Secret exists
         *
         * @param secret secret entity
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool SecretExists(const Entity::SecretsManager::Secret &secret) const override;

        /**
         * @brief Secret exists by ID
         *
         * @param secretId secret ID
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool SecretExists(const std::string &secretId) const override;

        /**
         * @brief Secret exists by ARN
         *
         * @param arn secret ARN
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool SecretExistsByArn(const std::string &arn) const override;

        /**
         * @brief Returns the secret by oid
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret GetSecretById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns the secret by userPoolId
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret GetSecretById(const std::string &oid) const override;

        /**
         * @brief Returns the secret by AWS ARN
         *
         * @param arn secret ARN
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret GetSecretByArn(const std::string &arn) const override;

        /**
         * @brief Returns the secret by region and name.
         *
         * @param region AWS region
         * @param name secret name
         * @return secret entity
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret GetSecretByRegionName(const std::string &region, const std::string &name) const override;

        /**
         * @brief Returns the secret by secret ID.
         *
         * @param secretId secret ID
         * @return secret entity
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret GetSecretBySecretId(const std::string &secretId) const override;

        /**
         * @brief Creates a new secret in the secrets collection
         *
         * @param secret secret entity
         * @return created secret entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret CreateSecret(Entity::SecretsManager::Secret &secret) const override;

        /**
         * @brief Updates an existing secret
         *
         * @param secret secret entity
         * @return updated secret entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret UpdateSecret(Entity::SecretsManager::Secret &secret) const override;

        /**
         * @brief Creates or updates a secret in the secret collection
         *
         * @param secret secret entity
         * @return created secret entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SecretsManager::Secret CreateOrUpdateSecret(Entity::SecretsManager::Secret &secret) const override;

        /**
         * @brief Returns a list of secrets
         *
         * @return list of available secrets
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SecretsManager::SecretList ListSecrets() const override;

        /**
         * @brief Returns the total number of secrets
         *
         * @param region AWS region
         * @return total number of secrets
         * @throws DatabaseException
         */
        [[nodiscard]]
        long CountSecrets(const std::string &region = {}) const override;

        /**
         * @brief Delete a secret.
         *
         * @param secret secret entity
         * @throws DatabaseException
         */
        void DeleteSecret(const Entity::SecretsManager::Secret &secret) const override;

        /**
         * @brief Delete all secret.
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long DeleteAllSecrets() const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SecretsManager"};

        /**
         * Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * Bucket collection name
         */
        static constexpr auto _parameterCollectionName = "secretsmanager_parameter";
    };

}// namespace Awsmock::Database
