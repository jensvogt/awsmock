//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/ApiGatewayMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex ApiGatewayMemoryDb::_apiGatewayMutex;

    bool ApiGatewayMemoryDb::ApiKeyExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_apiKeys,
                                    [region, name](const std::pair<std::string, Entity::ApiGateway::Key> &key) {
                                        return key.second.region == region && key.second.name == name;
                                    }) != _apiKeys.end();
    }

    bool ApiGatewayMemoryDb::ApiKeyExists(const std::string &id) const {

        return std::ranges::find_if(_apiKeys,
                                    [id](const std::pair<std::string, Entity::ApiGateway::Key> &key) {
                                        return key.second.id == id;
                                    }) != _apiKeys.end();
    }

    Entity::ApiGateway::Key ApiGatewayMemoryDb::CreateKey(const Entity::ApiGateway::Key &key) {
        boost::mutex::scoped_lock lock(_apiGatewayMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _apiKeys[oid] = key;
        log_trace << "Application created, oid: " << oid;
        return _apiKeys[oid];
    }

    std::vector<Entity::ApiGateway::Key> ApiGatewayMemoryDb::GetApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, long limit) {
        boost::mutex::scoped_lock lock(_apiGatewayMutex);

        std::vector<Entity::ApiGateway::Key> result;

        // Get values
        for (auto &val: _apiKeys | std::views::values) {
            result.push_back(val);
        }

        auto q = Core::from(result);
        q = q.order_by([](const Entity::ApiGateway::Key &key1, const Entity::ApiGateway::Key &key2) { return key1.id < key2.id; });

        if (!nameQuery.empty()) {
            q.where([nameQuery](const Entity::ApiGateway::Key &item) { return item.name == nameQuery; });
        }
        if (!customerId.empty()) {
            q.where([customerId](const Entity::ApiGateway::Key &item) { return item.customerId == customerId; });
        }
        if (!position.empty()) {
            q.where([position](const Entity::ApiGateway::Key &item) { return item.id > position; });
        }
        return q.to_vector();
    }

    Entity::ApiGateway::Key ApiGatewayMemoryDb::GetApiKeyById(const std::string &id) {
        boost::mutex::scoped_lock lock(_apiGatewayMutex);

        const auto it = std::ranges::find_if(_apiKeys, [id](const std::pair<std::string, Entity::ApiGateway::Key> &key) {
            return key.first == id;
        });

        if (it != _apiKeys.end()) {
            return it->second;
        }

        log_warning << "API Key not found, id: " << id;
        return {};
    }

    Entity::ApiGateway::Key ApiGatewayMemoryDb::UpdateApiKey(const Entity::ApiGateway::Key &key) {
        boost::mutex::scoped_lock lock(_apiGatewayMutex);

        std::string keyId = key.id;
        const auto it = std::ranges::find_if(_apiKeys,
                                             [keyId](const std::pair<std::string, Entity::ApiGateway::Key> &key) {
                                                 return key.second.id == keyId;
                                             });
        if (it != _apiKeys.end()) {
            _apiKeys[it->first] = key;
            return _apiKeys[it->first];
        }
        log_warning << "Key not found, id: " << keyId;
        return key;
    }

    void ApiGatewayMemoryDb::ImportApiKey(Entity::ApiGateway::Key &key) {

        if (ApiKeyExists(key.id)) {
            key = UpdateApiKey(key);
        }
        CreateKey(key);
    }

    long ApiGatewayMemoryDb::CountApiKeys() const {

        return static_cast<long>(_apiKeys.size());
    }

    void ApiGatewayMemoryDb::DeleteKey(const std::string &id) {
        boost::mutex::scoped_lock lock(_apiGatewayMutex);

        const auto count = std::erase_if(_apiKeys, [id](const auto &item) {
            auto const &[key, value] = item;
            return value.id == id;
        });
        log_debug << "API gateway key deleted, count: " << count;
    }

    std::vector<Entity::ApiGateway::Key> ApiGatewayMemoryDb::ListApiKeyCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        boost::mutex::scoped_lock lock(_apiGatewayMutex);

        std::vector<Entity::ApiGateway::Key> values;

        // Get values
        for (auto &val: _apiKeys | std::views::values) {
            values.push_back(val);
        }

        auto q = Core::from(values);
        if (!sortColumns.empty()) {
            for (const auto &[column, sortDirection]: sortColumns) {
                if (column == "id") {
                    q = q.order_by([](const Entity::ApiGateway::Key &key1, const Entity::ApiGateway::Key &key2) { return key1.id < key2.id; });
                }
                if (column == "name") {
                    q = q.order_by([](const Entity::ApiGateway::Key &key1, const Entity::ApiGateway::Key &key2) { return key1.name < key2.name; });
                }
            }
        }

        if (!prefix.empty()) {
            q.where([prefix](const Entity::ApiGateway::Key &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        auto resultVector = q.to_vector();
        return {resultVector.begin() + pageSize * pageIndex, resultVector.begin() + pageSize * (pageIndex + 1)};
    }

}// namespace AwsMock::Database
