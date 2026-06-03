//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/kms/KMSMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex KMSMemoryRepository::_keyMutex;

    bool KMSMemoryRepository::keyExists(const std::string &keyId) const {

        return std::ranges::find_if(_keys, [keyId](const std::pair<std::string, Entity::KMS::Key> &topic) {
            return topic.second.keyId == keyId;
        }) != _keys.end();
    }

    Entity::KMS::Key KMSMemoryRepository::getKeyById(const std::string &oid) const {

        const auto it = std::ranges::find_if(_keys, [oid](const std::pair<std::string, Entity::KMS::Key> &topic) {
            return topic.first == oid;
        });

        if (it != _keys.end()) {
            it->second.oid = oid;
            return it->second;
        }

        log_warning << "Key not found, oid: " << oid;
        return {};
    }

    Entity::KMS::Key KMSMemoryRepository::getKeyById(const bsoncxx::oid &oid) const {
        return getKeyById(oid.to_string());
    }
    
    Entity::KMS::Key KMSMemoryRepository::getKeyByKeyId(const std::string &keyId) const {

        const auto it = std::ranges::find_if(_keys, [keyId](const std::pair<std::string, Entity::KMS::Key> &topic) {
            return topic.second.keyId == keyId;
        });

        if (it != _keys.end()) {
            it->second.oid = it->first;
            return it->second;
        }

        log_warning << "Key not found, keyId: " << keyId;
        return {};
    }

    Entity::KMS::KeyList KMSMemoryRepository::listKeys(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        Entity::KMS::KeyList keyList;

        if (region.empty()) {

            for (const auto &val: _keys | std::views::values) {
                keyList.emplace_back(val);
            }

        } else {

            for (const auto &val: _keys | std::views::values) {
                if (val.region == region) {
                    keyList.emplace_back(val);
                }
            }
        }

        log_trace << "Got key list, size: " << keyList.size();
        return keyList;
    }

    long KMSMemoryRepository::countKeys() const {
        return static_cast<long>(_keys.size());
    }

    Entity::KMS::Key KMSMemoryRepository::createKey(Entity::KMS::Key &topic) const {
        boost::mutex::scoped_lock lock(_keyMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _keys[oid] = topic;
        log_trace << "Key created, oid: " << oid;
        return _keys[oid];
    }

    Entity::KMS::Key KMSMemoryRepository::updateKey(Entity::KMS::Key &key) const {
        boost::mutex::scoped_lock lock(_keyMutex);

        std::string keyId = key.keyId;
        const auto it = std::ranges::find_if(_keys,
                                             [keyId](const std::pair<std::string, Entity::KMS::Key> &key) {
                                                 return key.second.keyId == keyId;
                                             });
        if (it != _keys.end()) {
            _keys[it->first] = key;
            return _keys[it->first];
        }
        log_warning << "Key not found, keyId: " << keyId;
        return key;
    }

    Entity::KMS::Key KMSMemoryRepository::upsertKey(Entity::KMS::Key &key) const {
        if (keyExists(key.keyId)) {
            return updateKey(key);
        }
        return createKey(key);
    }

    void KMSMemoryRepository::deleteKey(const Entity::KMS::Key &key) const {
        boost::mutex::scoped_lock lock(_keyMutex);

        std::string keyId = key.keyId;
        const auto count = std::erase_if(_keys, [keyId](const auto &item) {
            auto const &[k, v] = item;
            return v.keyId == keyId;
        });
        log_debug << "Key deleted, count: " << count;
    }

    long KMSMemoryRepository::deleteAllKeys() const {
        boost::mutex::scoped_lock lock(_keyMutex);
        const long deleted = static_cast<long>(_keys.size());
        _keys.clear();
        log_debug << "All KMS keys deleted, count: " << deleted;
        return deleted;
    }

} // namespace Awsmock::Database
