//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SECRETSMANAGER_MEMORYDB_H
#define AWSMOCK_REPOSITORY_SECRETSMANAGER_MEMORYDB_H

// C++ standard includes
#include <string>
#include <vector>
#include <iostream>

// Poco includes
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <Poco/SingletonHolder.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Net/HTTPResponse.h>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Configuration.h>
#include <awsmock/core/DatabaseException.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/repository/Database.h>
#include <awsmock/entity/secretsmanager/Secret.h>

namespace AwsMock::Database {

  class SecretsManagerMemoryDb {

  public:

    /**
     * Constructor
     */
    SecretsManagerMemoryDb();

    /**
     * Singleton instance
     */
    static SecretsManagerMemoryDb &instance() {
      static Poco::SingletonHolder<SecretsManagerMemoryDb> sh;
      return *sh.get();
    }

    /**
     * Secret exists
     *
     * @param region AWS region
     * @param name secret name
     * @return true if secret exists
     * @throws DatabaseException
     */
    bool SecretExists(const std::string &region, const std::string &name);

    /**
     * Secret exists
     *
     * @param secret secret entity
     * @return true if secret exists
     * @throws DatabaseException
     */
    bool SecretExists(const Entity::SecretsManager::Secret &secret);

    /**
     * Secret exists
     *
     * @param secretId secret ID
     * @return true if secret exists
     * @throws DatabaseException
     */
    bool SecretExists(const std::string &secretId);

    /**
     * Returns the secret by userPoolId
     *
     * @param oid secret oid
     * @return secret, if existing
     * @throws DatabaseException
     */
    Entity::SecretsManager::Secret GetSecretById(const std::string &oid);

    /**
     * Returns the secret by region and name.
     *
     * @param region AWS region
     * @param name secret name
     * @return secret entity
     */
    Entity::SecretsManager::Secret GetSecretByRegionName(const std::string &region, const std::string &name);

    /**
     * Returns the secret by secret ID.
     *
     * @param secretId secret ID
     * @return secret entity
     */
    Entity::SecretsManager::Secret GetSecretBySecretId(const std::string &secretId);

    /**
     * Creates a new secret in the secrets manager collection
     *
     * @param secret secret entity
     * @return created secret entity
     * @throws DatabaseException
     */
    Entity::SecretsManager::Secret CreateSecret(const Entity::SecretsManager::Secret &secret);

    /**
     * Updates an existing secret
     *
     * @param secret secret entity
     * @return updated secret entity
     * @throws DatabaseException
     */
    Entity::SecretsManager::Secret UpdateSecret(const Entity::SecretsManager::Secret &secret);

    /**
     * Returns a list of secrets
     *
     * @return list of available secrets
     * @throws DatabaseException
     */
    Entity::SecretsManager::SecretList ListSecrets();

    /**
     * Delete a secret.
     *
     * @param secret secret entity
     * @throws DatabaseException
     */
    void DeleteSecret(const Entity::SecretsManager::Secret &secret);

  private:

    /**
     * Logger
     */
    Core::LogStream _logger;

    /**
     * Lambda map
     */
    std::map<std::string, Entity::SecretsManager::Secret> _secrets{};

    /**
     * Lambda mutex
     */
    Poco::Mutex _secretMutex;
  };

} // namespace AwsMock::Database

#endif // AWSMOCK_REPOSITORY_SECRETSMANAGER_MEMORYDB_H