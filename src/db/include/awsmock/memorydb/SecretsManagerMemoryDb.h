//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SECRETS_MANAGER_MEMORYDB_H
#define AWSMOCK_REPOSITORY_SECRETS_MANAGER_MEMORYDB_H

// C++ standard includes
#include <string>
#include <vector>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/secretsmanager/Secret.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * Secrets manager in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SecretsManagerMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        SecretsManagerMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static SecretsManagerMemoryDb &instance() {
            static SecretsManagerMemoryDb secretsManagerMemoryDb;
            return secretsManagerMemoryDb;
        }

        /**
         * @brief Secret exists by name and region
         *
         * @param region AWS region
         * @param name secret name
         * @return true if secret exists
         * @throws DatabaseException
         */
        bool SecretExists(const std::string &region, const std::string &name);

        /**
         * @brief Secret exists by secret
         *
         * @param secret secret entity
         * @return true if secret exists
         * @throws DatabaseException
         */
        bool SecretExists(const Entity::SecretsManager::Secret &secret);

        /**
         * @brief Secret exists by secret ID
         *
         * @param secretId secret ID
         * @return true if secret exists
         * @throws DatabaseException
         */
        bool SecretExists(const std::string &secretId);

        /**
         * @brief Secret exists by ARN
         *
         * @param arn secret ARN
         * @return true if secret exists
         * @throws DatabaseException
         */
        bool SecretExistsByArn(const std::string &arn) const;

        /**
         * @brief Returns the secret by userPoolId
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret GetSecretById(const std::string &oid);

        /**
         * @brief Returns the secret by region and name.
         *
         * @param region AWS region
         * @param name secret name
         * @return secret entity
         */
        Entity::SecretsManager::Secret GetSecretByRegionName(const std::string &region, const std::string &name);

        /**
         * @brief Returns the secret by secret ID.
         *
         * @param secretId secret ID
         * @return secret entity
         */
        Entity::SecretsManager::Secret GetSecretBySecretId(const std::string &secretId);

        /**
         * @brief Returns the secret by AWS ARN
         *
         * @param arn secret ARN
         * @return secret, if existing
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret GetSecretByArn(const std::string &arn);

        /**
         * @brief Creates a new secret in the secrets manager collection
         *
         * @param secret secret entity
         * @return created secret entity
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret CreateSecret(const Entity::SecretsManager::Secret &secret);

        /**
         * @brief Updates an existing secret
         *
         * @param secret secret entity
         * @return updated secret entity
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret UpdateSecret(const Entity::SecretsManager::Secret &secret);

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
        void DeleteSecret(const Entity::SecretsManager::Secret &secret);

        /**
         * @brief Delete all secret.
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        long DeleteAllSecrets();

      private:

        /**
         * Lambda map
         */
        std::map<std::string, Entity::SecretsManager::Secret> _secrets{};

        /**
         * Lambda mutex
         */
        static boost::mutex _secretMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SECRETS_MANAGER_MEMORYDB_H
