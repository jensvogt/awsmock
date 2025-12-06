//
// Created by vogje01 on 11/19/23.
//

#include "awsmock/entity/apigateway/RestApi.h"


#include <awsmock/memorydb/ApiGatewayMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex ApiGatewayMemoryDb::_apiKeyMutex;
    boost::mutex ApiGatewayMemoryDb::_restApiMutex;

    // ========================================================================================================================
    // API key
    // ========================================================================================================================
    bool ApiGatewayMemoryDb::ApiKeyExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_apiKeys,
                                    [region, name](const std::pair<std::string, Entity::ApiGateway::ApiKey> &key) {
                                        return key.second.region == region && key.second.name == name;
                                    }) != _apiKeys.end();
    }

    bool ApiGatewayMemoryDb::ApiKeyExists(const std::string &id) const {

        return std::ranges::find_if(_apiKeys,
                                    [id](const std::pair<std::string, Entity::ApiGateway::ApiKey> &key) {
                                        return key.second.id == id;
                                    }) != _apiKeys.end();
    }

    Entity::ApiGateway::ApiKey ApiGatewayMemoryDb::CreateKey(const Entity::ApiGateway::ApiKey &key) {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _apiKeys[oid] = key;
        log_trace << "Application created, oid: " << oid;
        return _apiKeys[oid];
    }

    std::vector<Entity::ApiGateway::ApiKey> ApiGatewayMemoryDb::GetApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, long limit) {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        std::vector<Entity::ApiGateway::ApiKey> result;

        // Get values
        for (auto &val: _apiKeys | std::views::values) {
            result.push_back(val);
        }

        auto q = Core::from(result);
        q = q.order_by([](const Entity::ApiGateway::ApiKey &key1, const Entity::ApiGateway::ApiKey &key2) { return key1.id < key2.id; });

        if (!nameQuery.empty()) {
            q.where([nameQuery](const Entity::ApiGateway::ApiKey &item) { return item.name == nameQuery; });
        }
        if (!customerId.empty()) {
            q.where([customerId](const Entity::ApiGateway::ApiKey &item) { return item.customerId == customerId; });
        }
        if (!position.empty()) {
            q.where([position](const Entity::ApiGateway::ApiKey &item) { return item.id > position; });
        }
        return q.to_vector();
    }

    Entity::ApiGateway::ApiKey ApiGatewayMemoryDb::GetApiKeyById(const std::string &id) {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        const auto it = std::ranges::find_if(_apiKeys, [id](const std::pair<std::string, Entity::ApiGateway::ApiKey> &key) {
            return key.first == id;
        });

        if (it != _apiKeys.end()) {
            return it->second;
        }

        log_warning << "API Key not found, id: " << id;
        return {};
    }

    Entity::ApiGateway::ApiKey ApiGatewayMemoryDb::UpdateApiKey(const Entity::ApiGateway::ApiKey &key) {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        std::string keyId = key.id;
        const auto it = std::ranges::find_if(_apiKeys,
                                             [keyId](const std::pair<std::string, Entity::ApiGateway::ApiKey> &key) {
                                                 return key.second.id == keyId;
                                             });
        if (it != _apiKeys.end()) {
            _apiKeys[it->first] = key;
            return _apiKeys[it->first];
        }
        log_warning << "Key not found, id: " << keyId;
        return key;
    }

    void ApiGatewayMemoryDb::ImportApiKey(Entity::ApiGateway::ApiKey &key) {

        if (ApiKeyExists(key.id)) {
            key = UpdateApiKey(key);
        }
        CreateKey(key);
    }

    long ApiGatewayMemoryDb::CountApiKeys() const {

        return static_cast<long>(_apiKeys.size());
    }

    void ApiGatewayMemoryDb::DeleteKey(const std::string &id) {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        const auto count = std::erase_if(_apiKeys, [id](const auto &item) {
            auto const &[key, value] = item;
            return value.id == id;
        });
        log_debug << "API gateway key deleted, count: " << count;
    }

    long ApiGatewayMemoryDb::DeleteAllKeys() {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        const long count = _apiKeys.size();
        _apiKeys.clear();
        log_debug << "API gateway keys deleted, count: " << count;
        return count;
    }

    // ========================================================================================================================
    // REST API
    // ========================================================================================================================
    bool ApiGatewayMemoryDb::RestApiExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_restApis,
                                    [region, name](const std::pair<std::string, Entity::ApiGateway::RestApi> &restApi) {
                                        return restApi.second.region == region && restApi.second.name == name;
                                    }) != _restApis.end();
    }

    bool ApiGatewayMemoryDb::RestApiExists(const std::string &id) const {

        return std::ranges::find_if(_restApis,
                                    [id](const std::pair<std::string, Entity::ApiGateway::RestApi> &restApi) {
                                        return restApi.first == id;
                                    }) != _restApis.end();
    }

    Entity::ApiGateway::RestApi ApiGatewayMemoryDb::CreateRestApi(const Entity::ApiGateway::RestApi &restApi) {
        boost::mutex::scoped_lock lock(_restApiMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _restApis[oid] = restApi;
        log_trace << "REST API created, oid: " << oid;
        return _restApis[oid];
    }

    // ========================================================================================================================
    // AwsMock internal
    // ========================================================================================================================
    std::vector<Entity::ApiGateway::ApiKey> ApiGatewayMemoryDb::ListApiKeyCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        std::vector<Entity::ApiGateway::ApiKey> values;

        // Get values
        for (auto &val: _apiKeys | std::views::values) {
            values.push_back(val);
        }

        auto q = Core::from(values);
        if (!sortColumns.empty()) {
            for (const auto &[column, sortDirection]: sortColumns) {
                if (column == "id") {
                    q = q.order_by([](const Entity::ApiGateway::ApiKey &key1, const Entity::ApiGateway::ApiKey &key2) { return key1.id < key2.id; });
                }
                if (column == "name") {
                    q = q.order_by([](const Entity::ApiGateway::ApiKey &key1, const Entity::ApiGateway::ApiKey &key2) { return key1.name < key2.name; });
                }
            }
        }

        if (!prefix.empty()) {
            q.where([prefix](const Entity::ApiGateway::ApiKey &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        auto resultVector = q.to_vector();
        return {resultVector.begin() + pageSize * pageIndex, resultVector.begin() + pageSize * (pageIndex + 1)};
    }

    std::vector<Entity::ApiGateway::RestApi> ApiGatewayMemoryDb::ListRestApiCounters(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        boost::mutex::scoped_lock lock(_restApiMutex);

        std::vector<Entity::ApiGateway::RestApi> values;

        // Get values
        for (auto &val: _restApis | std::views::values) {
            values.push_back(val);
        }

        auto q = Core::from(values);
        if (!sortColumns.empty()) {
            for (const auto &[column, sortDirection]: sortColumns) {
                if (column == "id") {
                    q = q.order_by([](const Entity::ApiGateway::RestApi &key1, const Entity::ApiGateway::RestApi &key2) { return key1.id < key2.id; });
                }
                if (column == "name") {
                    q = q.order_by([](const Entity::ApiGateway::RestApi &key1, const Entity::ApiGateway::RestApi &key2) { return key1.name < key2.name; });
                }
            }
        }

        if (!prefix.empty()) {
            q.where([prefix](const Entity::ApiGateway::RestApi &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        auto resultVector = q.to_vector();
        return {resultVector.begin() + pageSize * pageIndex, resultVector.begin() + pageSize * (pageIndex + 1)};
    }
}// namespace AwsMock::Database
