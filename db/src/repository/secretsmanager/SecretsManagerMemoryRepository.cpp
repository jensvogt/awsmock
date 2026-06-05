//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/secretsmanager/SecretsManagerMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex SecretsManagerMemoryRepository::_secretMutex;

    bool SecretsManagerMemoryRepository::SecretExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_secrets,
                                    [region, name](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                        return secret.second.region == region && secret.second.name == name;
                                    }) != _secrets.end();
    }

    bool SecretsManagerMemoryRepository::SecretExistsByArn(const std::string &arn) const {

        return std::ranges::find_if(_secrets,
                                    [arn](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                        return secret.second.arn == arn;
                                    }) != _secrets.end();
    }

    bool SecretsManagerMemoryRepository::SecretExists(const Entity::SecretsManager::Secret &secret) const {
        return SecretExists(secret.region, secret.name);
    }

    bool SecretsManagerMemoryRepository::SecretExists(const std::string &secretId) const {

        return std::ranges::find_if(_secrets,
                                    [secretId](const std::pair<std::string, Entity::SecretsManager::Secret> &secret) {
                                        return secret.second.secretId == secretId;
                                    }) != _secrets.end();
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::GetSecretById(bsoncxx::oid oid) const {
        return GetSecretById(oid.to_string());
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::GetSecretById(const std::string &oid) const {

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

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::GetSecretByRegionName(const std::string &region, const std::string &name) const {

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

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::GetSecretBySecretId(const std::string &secretId) const {

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

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::GetSecretByArn(const std::string &arn) const {

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

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::CreateSecret(Entity::SecretsManager::Secret &secret) const {
        boost::mutex::scoped_lock lock(_secretMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _secrets[oid] = secret;
        log_trace << "Secret created, oid: " << oid;
        return GetSecretById(oid);
    }

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::UpdateSecret(Entity::SecretsManager::Secret &secret) const {

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

    Entity::SecretsManager::Secret SecretsManagerMemoryRepository::CreateOrUpdateSecret(Entity::SecretsManager::Secret &secret) const {
        if (SecretExists(secret)) {
            return UpdateSecret(secret);
        }
        return CreateSecret(secret);
    }

    Entity::SecretsManager::SecretList SecretsManagerMemoryRepository::ListSecrets() const {

        Entity::SecretsManager::SecretList secretList;

        for (const auto &val: _secrets | std::views::values) {
            secretList.emplace_back(val);
        }

        log_trace << "Got secret list, size: " << secretList.size();
        return secretList;
    }

    long SecretsManagerMemoryRepository::CountSecrets(const std::string &region) const {

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

    void SecretsManagerMemoryRepository::DeleteSecret(const Entity::SecretsManager::Secret &secret) const {
        boost::mutex::scoped_lock lock(_secretMutex);

        std::string region = secret.region;
        std::string name = secret.name;
        const auto count = std::erase_if(_secrets, [region, name](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.name == name;
        });
        log_debug << "Secret deleted, count: " << count;
    }

    long SecretsManagerMemoryRepository::DeleteAllSecrets() const {
        boost::mutex::scoped_lock lock(_secretMutex);
        const long deleted = static_cast<long>(_secrets.size());
        _secrets.clear();
        log_debug << "Secrets deleted, count: " << deleted;
        return deleted;
    }

}// namespace Awsmock::Database