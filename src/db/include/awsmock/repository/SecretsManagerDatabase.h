//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SECRETSMANAGER_DATABASE_H
#define AWSMOCK_REPOSITORY_SECRETSMANAGER_DATABASE_H

// C++ standard includes
#include <iostream>
#include <string>
#include <vector>

// Poco includes
#include <Poco/Net/HTTPResponse.h>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include "awsmock/core/exception/DatabaseException.h"
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
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
        bool SecretExists(const std::string &region, const std::string &name);

        /**
         * @brief Secret exists
         *
         * @param secret secret entity
         * @return true if secret exists
         * @throws DatabaseException
         */
        bool SecretExists(const Entity::SecretsManager::Secret &secret);

        /**
         * @brief Secret exists
         *
         * @param secretId secret ID
         * @return true if secret exists
         * @throws DatabaseException
         */
        bool SecretExists(const std::string &secretId);

        /**
         * @brief Returns the secret by oid
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret GetSecretById(bsoncxx::oid oid);

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
         * @brief Creates a new secret in the secrets collection
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
         * @brief Creates or updates a secret in the secrets collection
         *
         * @param secret secret entity
         * @return created secret entity
         * @throws DatabaseException
         */
        Entity::SecretsManager::Secret CreateOrUpdateSecret(const Entity::SecretsManager::Secret &secret);

        /**
         * @brief Returns a list of secrets
         *
         * @return list of available secrets
         * @throws DatabaseException
         */
        Entity::SecretsManager::SecretList ListSecrets();

        /**
         * @brief Returns the total number of secrets
         *
         * @param region AWS region
         * @return total number of secrets
         * @throws DatabaseException
         */
        long CountSecrets(const std::string &region = {});

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
         * @throws DatabaseException
         */
        void DeleteAllSecrets();

      private:

        /**
         * Use MongoDB
         */
        bool _useDatabase;

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
