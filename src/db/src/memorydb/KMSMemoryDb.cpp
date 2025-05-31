//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/KMSMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex KMSMemoryDb::_keyMutex;

    bool KMSMemoryDb::KeyExists(const std::string &keyId) {

        return std::ranges::find_if(_keys, [keyId](const std::pair<std::string, Entity::KMS::Key> &topic) {
                   return topic.second.keyId == keyId;
               }) != _keys.end();
    }

    Entity::KMS::Key KMSMemoryDb::GetKeyById(const std::string &oid) {

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

    Entity::KMS::Key KMSMemoryDb::GetKeyByKeyId(const std::string &keyId) {

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

    Entity::KMS::KeyList KMSMemoryDb::ListKeys(const std::string &region) const {

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

    long KMSMemoryDb::CountKeys() const {

        return static_cast<long>(_keys.size());
    }

    Entity::KMS::Key KMSMemoryDb::CreateKey(const Entity::KMS::Key &topic) {
        boost::mutex::scoped_lock lock(_keyMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _keys[oid] = topic;
        log_trace << "Key created, oid: " << oid;
        return GetKeyById(oid);
    }

    Entity::KMS::Key KMSMemoryDb::UpdateKey(const Entity::KMS::Key &key) {
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

    void KMSMemoryDb::DeleteKey(const Entity::KMS::Key &key) {
        boost::mutex::scoped_lock lock(_keyMutex);

        std::string keyId = key.keyId;
        const auto count = std::erase_if(_keys, [keyId](const auto &item) {
            auto const &[k, v] = item;
            return v.keyId == keyId;
        });
        log_debug << "Key deleted, count: " << count;
    }

    long KMSMemoryDb::DeleteAllKeys() {
        boost::mutex::scoped_lock lock(_keyMutex);
        const long deleted = static_cast<long>(_keys.size());
        _keys.clear();
        log_debug << "All KMS keys deleted, count: " << deleted;
        return deleted;
    }

}// namespace AwsMock::Database