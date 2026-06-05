//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/secretsmanager/Secret.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for secrets manager repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * secret-manager-related data.
     */
    class ISecretsManagerRepository {

      public:

        /**
         * @brief Virtual destructor for the IS3Repository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~ISecretsManagerRepository() = default;

        /**
         * @brief Secret exists
         *
         * @param region AWS region
         * @param name secret name
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool SecretExists(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Secret exists
         *
         * @param secret secret entity
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool SecretExists(const Entity::SecretsManager::Secret &secret) const = 0;

        /**
         * @brief Secret exists by ID
         *
         * @param secretId secret ID
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool SecretExists(const std::string &secretId) const = 0;

        /**
         * @brief Secret exists by ARN
         *
         * @param arn secret ARN
         * @return true if secret exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool SecretExistsByArn(const std::string &arn) const = 0;

        /**
         * @brief Returns the secret by oid
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret GetSecretById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Returns the secret by userPoolId
         *
         * @param oid secret oid
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret GetSecretById(const std::string &oid) const = 0;

        /**
         * @brief Returns the secret by AWS ARN
         *
         * @param arn secret ARN
         * @return secret, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret GetSecretByArn(const std::string &arn) const = 0;

        /**
         * @brief Returns the secret by region and name.
         *
         * @param region AWS region
         * @param name secret name
         * @return secret entity
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret GetSecretByRegionName(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Returns the secret by secret ID.
         *
         * @param secretId secret ID
         * @return secret entity
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret GetSecretBySecretId(const std::string &secretId) const = 0;

        /**
         * @brief Creates a new secret in the secrets collection
         *
         * @param secret secret entity
         * @return created a secret entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret CreateSecret(Entity::SecretsManager::Secret &secret) const = 0;

        /**
         * @brief Updates an existing secret
         *
         * @param secret secret entity
         * @return updated secret entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret UpdateSecret(Entity::SecretsManager::Secret &secret) const = 0;

        /**
         * @brief Creates or updates a secret in the secret collection
         *
         * @param secret secret entity
         * @return created secret entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::Secret CreateOrUpdateSecret(Entity::SecretsManager::Secret &secret) const = 0;

        /**
         * @brief Returns a list of secrets
         *
         * @return list of available secrets
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SecretsManager::SecretList ListSecrets() const = 0;

        /**
         * @brief Returns the total number of secrets
         *
         * @param region AWS region
         * @return total number of secrets
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long CountSecrets(const std::string &region = {}) const = 0;

        /**
         * @brief Delete a secret.
         *
         * @param secret secret entity
         * @throws DatabaseException
         */
        virtual void DeleteSecret(const Entity::SecretsManager::Secret &secret) const = 0;

        /**
         * @brief Delete all secret.
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long DeleteAllSecrets() const = 0;
    };

}// namespace Awsmock::Database