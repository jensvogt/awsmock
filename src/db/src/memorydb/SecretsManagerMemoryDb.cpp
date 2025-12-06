//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/SecretsManagerMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex SecretsManagerMemoryDb::_secretMutex;

    bool SecretsManagerMemoryDb::SecretExists(const std::string &region, const std::string &name) {

        return std::ranges::find_if(_secrets,
                                    [region, name](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                        return secret.second.region == region && secret.second.name == name;
                                    }) != _secrets.end();
    }

    bool SecretsManagerMemoryDb::SecretExistsByArn(const std::string &arn) const {

        return std::ranges::find_if(_secrets,
                                    [arn](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                        return secret.second.arn == arn;
                                    }) != _secrets.end();
    }

    bool SecretsManagerMemoryDb::SecretExists(const Entity::SecretsManager::Secret &secret) {
        return SecretExists(secret.region, secret.name);
    }

    bool SecretsManagerMemoryDb::SecretExists(const std::string &secretId) {

        return std::ranges::find_if(_secrets,
                                    [secretId](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                        return secret.second.secretId == secretId;
                                    }) != _secrets.end();
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryDb::GetSecretById(const std::string &oid) {

        const auto it = std::ranges::find_if(_secrets,
                                             [oid](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                                 return secret.first == oid;
                                             });

        if (it != _secrets.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryDb::GetSecretByRegionName(const std::string &region, const std::string &name) {

        Entity::SecretsManager::Secret result;

        const auto it = std::ranges::find_if(_secrets,
                                             [region, name](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                                 return secret.second.region == region && secret.second.name == name;
                                             });

        if (it != _secrets.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryDb::GetSecretBySecretId(const std::string &secretId) {

        Entity::SecretsManager::Secret result;

        const auto it = std::ranges::find_if(_secrets,
                                             [secretId](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                                 return secret.second.secretId == secretId;
                                             });

        if (it != _secrets.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryDb::GetSecretByArn(const std::string &arn) {

        Entity::SecretsManager::Secret result;

        const auto it = std::ranges::find_if(_secrets,
                                             [arn](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                                 return secret.second.arn == arn;
                                             });

        if (it != _secrets.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryDb::CreateSecret(const Entity::SecretsManager::Secret &secret) {
        boost::mutex::scoped_lock lock(_secretMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _secrets[oid] = secret;
        log_trace << "Secret created, oid: " << oid;
        return GetSecretById(oid);
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryDb::UpdateSecret(const Entity::SecretsManager::Secret &secret) {

        boost::mutex::scoped_lock lock(_secretMutex);

        std::string secretId = secret.secretId;
        const auto it = std::ranges::find_if(_secrets,
                                             [secretId](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                                 return secret.second.secretId == secretId;
                                             });
        if (it != _secrets.end()) {
          _secrets[it->first] = secret;
            return _secrets[it->first];
        }
        return {};
    }

    Entity::SecretsManager::SecretList SecretsManagerMemoryDb::ListSecrets() const {

        Entity::SecretsManager::SecretList secretList;

        for (const auto &val: _secrets | std::views::values) {
            secretList.emplace_back(val);
        }

        log_trace << "Got secret list, size: " << secretList.size();
        return secretList;
    }

    long SecretsManagerMemoryDb::CountSecrets(const std::string &region) const {

        long count = 0;
        if (region.empty()) {

            return static_cast<long>(_secrets.size());
        }
        for (const auto &val: _secrets | std::views::values) {
            if (val.region == region) {
                count++;
            }
        }
        return count;
    }

    void SecretsManagerMemoryDb::DeleteSecret(const Entity::SecretsManager::Secret &secret) {
        boost::mutex::scoped_lock lock(_secretMutex);

        std::string region = secret.region;
        std::string name = secret.name;
        const auto count = std::erase_if(_secrets, [region, name](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.name == name;
        });
        log_debug << "Secret deleted, count: " << count;
    }

    long SecretsManagerMemoryDb::DeleteAllSecrets() {
        boost::mutex::scoped_lock lock(_secretMutex);
        const long deleted = _secrets.size();
        _secrets.clear();
        log_debug << "Secrets deleted, count: " << deleted;
        return deleted;
    }

}// namespace AwsMock::Database