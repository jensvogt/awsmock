//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SECRETSMANAGER_DATABASE_H
#define AWSMOCK_REPOSITORY_SECRETSMANAGER_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/secretsmanager/Secret.h>
#include <awsmock/memorydb/SecretsManagerMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief Secrets manager MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit SecretsManagerDatabase();

        /**
         * @brief Singleton instance
         */
        static SecretsManagerDatabase &instance() {
            static SecretsManagerDatabase secretsManagerDatabase;
            return secretsManagerDatabase;
        }

        /**
         * @brief Secret exists
         *
         * @param region AWS region
         * @param name secret name
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool SecretExists(const std::string &region, const std::string &name) const;

        /**
         * @brief Secret exists
         *
         * @param secret secret entity
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool SecretExists(const Entity::SecretsManager::Secret &secret) const;

        /**
         * @brief Secret exists by ID
         *
         * @param secretId secret ID
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool SecretExists(const std::string &secretId) const;

        /**
         * @brief Secret exists by ARN
         *
         * @param arn secret ARN
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool SecretExistsByArn(const std::string &arn) const;

        /**
         * @brief Returns the secret by oid
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SecretsManager::Secret GetSecretById(bsoncxx::oid oid) const;

        /**
         * @brief Returns the secret by userPoolId
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SecretsManager::Secret GetSecretById(const std::string &oid) const;

        /**
         * @brief Returns the secret by AWS ARN
         *
         * @param arn secret ARN
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SecretsManager::Secret GetSecretByArn(const std::string &arn) const;

        /**
         * @brief Returns the secret by region and name.
         *
         * @param region AWS region
         * @param name secret name
         * @return secret entity
         */
        [[nodiscard]] Entity::SecretsManager::Secret GetSecretByRegionName(const std::string &region, const std::string &name) const;

        /**
         * @brief Returns the secret by secret ID.
         *
         * @param secretId secret ID
         * @return secret entity
         */
        [[nodiscard]] Entity::SecretsManager::Secret GetSecretBySecretId(const std::string &secretId) const;

        /**
         * @brief Creates a new secret in the secrets collection
         *
         * @param secret secret entity
         * @return created secret entity
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret CreateSecret(Entity::SecretsManager::Secret &secret) const;

        /**
         * @brief Updates an existing secret
         *
         * @param secret secret entity
         * @return updated secret entity
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret UpdateSecret(Entity::SecretsManager::Secret &secret) const;

        /**
         * @brief Creates or updates a secret in the secret collection
         *
         * @param secret secret entity
         * @return created secret entity
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret CreateOrUpdateSecret(Entity::SecretsManager::Secret &secret) const;

        /**
         * @brief Returns a list of secrets
         *
         * @return list of available secrets
         * @throws DatabaseException
         */
        Entity::SecretsManager::SecretList ListSecrets() const;

        /**
         * @brief Returns the total number of secrets
         *
         * @param region AWS region
         * @return total number of secrets
         * @throws DatabaseException
         */
        long CountSecrets(const std::string &region = {}) const;

        /**
         * @brief Delete a secret.
         *
         * @param secret secret entity
         * @throws DatabaseException
         */
        void DeleteSecret(const Entity::SecretsManager::Secret &secret) const;

        /**
         * @brief Delete all secret.
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        long DeleteAllSecrets() const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Database collection name
         */
        std::string _collectionName;

        /**
         * Lambda in-memory database
         */
        SecretsManagerMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SECRETSMANAGER_DATABASE_H
