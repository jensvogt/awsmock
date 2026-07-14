//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/apigateway/ApiGatewayMemoryRepository.h>

#include "awsmock/core/PagingUtils.h"

namespace Awsmock::Database {

    boost::mutex ApiGatewayMemoryRepository::_apiKeyMutex;
    boost::mutex ApiGatewayMemoryRepository::_restApiMutex;
    boost::mutex ApiGatewayMemoryRepository::_usagePlanMutex;

    logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

    // ========================================================================================================================
    // API key
    // ========================================================================================================================
    bool ApiGatewayMemoryRepository::apiKeyExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_apiKeys,
                                    [region, name](const std::pair<std::string, Entity::ApiGateway::ApiKey> &key) {
                                        return key.second.region == region && key.second.name == name;
                                    }) != _apiKeys.end();
    }

    bool ApiGatewayMemoryRepository::apiKeyExists(const std::string &id) const {

        return std::ranges::find_if(_apiKeys,
                                    [id](const std::pair<std::string, Entity::ApiGateway::ApiKey> &key) {
                                        return key.second.id == id;
                                    }) != _apiKeys.end();
    }

    Entity::ApiGateway::ApiKey ApiGatewayMemoryRepository::createKey(Entity::ApiGateway::ApiKey &key) const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        key.oid = Core::StringUtils::CreateRandomUuid();
        _apiKeys[key.oid] = key;
        log_trace << "Application created, oid: " << key.oid;
        return _apiKeys[key.oid];
    }

    std::vector<Entity::ApiGateway::ApiKey> ApiGatewayMemoryRepository::listApiKeys() const {
        return listApiKeys({}, {}, {}, 0);
    }

    std::vector<Entity::ApiGateway::ApiKey> ApiGatewayMemoryRepository::listApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, long limit) const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        auto q = Core::from(Core::NumberUtils::toVector(_apiKeys));
        q = q.order_by([](const Entity::ApiGateway::ApiKey &key1, const Entity::ApiGateway::ApiKey &key2) { return key1.id < key2.id; });

        if (!nameQuery.empty()) {
            q = q.where([nameQuery](const Entity::ApiGateway::ApiKey &item) { return item.name == nameQuery; });
        }
        if (!customerId.empty()) {
            q = q.where([customerId](const Entity::ApiGateway::ApiKey &item) { return item.customerId == customerId; });
        }
        if (!position.empty()) {
            q = q.where([position](const Entity::ApiGateway::ApiKey &item) { return item.id > position; });
        }
        return q.to_vector();
    }

    Entity::ApiGateway::ApiKey ApiGatewayMemoryRepository::getApiKeyById(const std::string &id) const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        const auto it = std::ranges::find_if(_apiKeys, [id](const std::pair<std::string, Entity::ApiGateway::ApiKey> &key) {
            return key.second.id == id;
        });

        if (it != _apiKeys.end()) {
            return it->second;
        }

        log_warning << "API Key not found, id: " << id;
        return {};
    }

    Entity::ApiGateway::ApiKey ApiGatewayMemoryRepository::updateApiKey(Entity::ApiGateway::ApiKey &key) const {
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

    void ApiGatewayMemoryRepository::importApiKey(Entity::ApiGateway::ApiKey &key) const {

        if (apiKeyExists(key.id)) {
            key = updateApiKey(key);
        } else {
            key = createKey(key);
        }
    }

    void ApiGatewayMemoryRepository::importRestApis(Entity::ApiGateway::RestApi &restApi) const {
        restApi = upsertRestApi(restApi);
    }

    long ApiGatewayMemoryRepository::countApiKeys() const {
        return static_cast<long>(_apiKeys.size());
    }

    void ApiGatewayMemoryRepository::deleteKey(const std::string &id) const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        const auto count = std::erase_if(_apiKeys, [id](const auto &item) {
            auto const &[key, value] = item;
            return value.id == id;
        });
        log_debug << "API gateway key deleted, count: " << count;
    }

    long ApiGatewayMemoryRepository::deleteAllKeys() const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        const long count = static_cast<long>(_apiKeys.size());
        _apiKeys.clear();
        log_debug << "API gateway keys deleted, count: " << count;
        return count;
    }

    // ========================================================================================================================
    // REST API
    // ========================================================================================================================
    bool ApiGatewayMemoryRepository::restApiExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_restApis,
                                    [&region, &name](const std::pair<const std::string, Entity::ApiGateway::RestApi> &restApi) {
                                        return restApi.second.region == region && restApi.second.name == name;
                                    }) != _restApis.end();
    }

    bool ApiGatewayMemoryRepository::restApiExists(const std::string &id) const {

        return std::ranges::find_if(_restApis,
                                    [&id](const std::pair<const std::string, Entity::ApiGateway::RestApi> &restApi) {
                                        return restApi.first == id;
                                    }) != _restApis.end();
    }

    bool ApiGatewayMemoryRepository::restApiExistsByRestApiId(const std::string &restApiId) const {

        return std::ranges::find_if(_restApis,
                                    [&restApiId](const std::pair<const std::string, Entity::ApiGateway::RestApi> &restApi) {
                                        return restApi.second.id == restApiId;
                                    }) != _restApis.end();
    }

    Entity::ApiGateway::RestApi ApiGatewayMemoryRepository::createRestApi(Entity::ApiGateway::RestApi &restApi) const {
        boost::mutex::scoped_lock lock(_restApiMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        restApi.oid = oid;
        _restApis[oid] = restApi;
        log_trace << "REST API created, oid: " << oid;
        return _restApis[oid];
    }

    Entity::ApiGateway::RestApi ApiGatewayMemoryRepository::upsertRestApi(Entity::ApiGateway::RestApi &restApi) const {
        boost::mutex::scoped_lock lock(_restApiMutex);

        const auto it = std::ranges::find_if(_restApis,
                                             [&restApi](const std::pair<const std::string, Entity::ApiGateway::RestApi> &entry) {
                                                 return entry.second.region == restApi.region && entry.second.id == restApi.id;
                                             });
        if (it != _restApis.end()) {
            restApi.oid = it->first;
            _restApis[it->first] = restApi;
            log_trace << "REST API updated, oid: " << it->first;
            return _restApis[it->first];
        }

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        restApi.oid = oid;
        _restApis[oid] = restApi;
        log_trace << "REST API created, oid: " << oid;
        return _restApis[oid];
    }

    std::vector<Entity::ApiGateway::RestApi> ApiGatewayMemoryRepository::listRestApis() const {
        return listRestApis({});
    }

    std::vector<Entity::ApiGateway::RestApi> ApiGatewayMemoryRepository::listRestApis(const std::string &region) const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        auto q = Core::from(Core::NumberUtils::toVector(_restApis));
        if (!region.empty()) {
            q = q.where([region](const Entity::ApiGateway::RestApi &item) { return item.region == region; });
        }
        return q.to_vector();
    }

    // ========================================================================================================================
    // AwsMock internal
    // ========================================================================================================================
    std::vector<Entity::ApiGateway::ApiKey> ApiGatewayMemoryRepository::listApiKeyCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        boost::mutex::scoped_lock lock(_apiKeyMutex);

        auto q = Core::from(Core::NumberUtils::toVector(_apiKeys));
        if (!sortColumns.empty()) {
            for (const auto &sc: sortColumns) {
                if (sc.column == "id") {
                    q = q.order_by([](const Entity::ApiGateway::ApiKey &key1, const Entity::ApiGateway::ApiKey &key2) { return key1.id < key2.id; });
                }
                if (sc.column == "name") {
                    q = q.order_by([](const Entity::ApiGateway::ApiKey &key1, const Entity::ApiGateway::ApiKey &key2) { return key1.name < key2.name; });
                }
            }
        }

        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::ApiGateway::ApiKey &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        const auto resultVector = q.to_vector();
        return Core::PageVector(resultVector, pageSize, pageIndex);
    }

    std::vector<Entity::ApiGateway::RestApi> ApiGatewayMemoryRepository::listRestApiCounters(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        boost::mutex::scoped_lock lock(_restApiMutex);

        auto q = Core::from(Core::NumberUtils::toVector(_restApis));
        if (!sortColumns.empty()) {
            for (const auto &sc: sortColumns) {
                if (sc.column == "id") {
                    q = q.order_by([](const Entity::ApiGateway::RestApi &key1, const Entity::ApiGateway::RestApi &key2) { return key1.id < key2.id; });
                }
                if (sc.column == "name") {
                    q = q.order_by([](const Entity::ApiGateway::RestApi &key1, const Entity::ApiGateway::RestApi &key2) { return key1.name < key2.name; });
                }
            }
        }

        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::ApiGateway::RestApi &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        const auto resultVector = q.to_vector();
        return Core::PageVector(resultVector, pageSize, pageIndex);
    }

    long ApiGatewayMemoryRepository::countRestApis(const std::string &region, const std::string &prefix) const {
        auto q = Core::from(Core::NumberUtils::toVector(_restApis));

        if (!region.empty()) {
            q = q.where([region](const Entity::ApiGateway::RestApi &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([region](const Entity::ApiGateway::RestApi &item) { return Core::StringUtils::StartsWith(item.name, region); });
        }
        return static_cast<long>(q.count());
    }

    Entity::ApiGateway::RestApi ApiGatewayMemoryRepository::getRestApi(const std::string &region, const std::string &name) const {

        const auto it = std::ranges::find_if(_restApis,
                                             [region, name](const std::pair<std::string, Entity::ApiGateway::RestApi> &restApi) {
                                                 return restApi.second.region == region && restApi.second.name == name;
                                             });
        if (it != _restApis.end()) {
            return it->second;
        }
        return {};
    }

    Entity::ApiGateway::RestApi ApiGatewayMemoryRepository::getRestApi(const std::string &restApiId) const {

        const auto it = std::ranges::find_if(_restApis,
                                             [restApiId](const std::pair<std::string, Entity::ApiGateway::RestApi> &restApi) {
                                                 return restApi.second.id == restApiId;
                                             });
        if (it != _restApis.end()) {
            return it->second;
        }
        return {};
    }

    long ApiGatewayMemoryRepository::deleteRestApi(const std::string &region, const std::string &name) const {
        boost::mutex::scoped_lock lock(_restApiMutex);

        const auto count = std::erase_if(_restApis, [region, name](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.name == name;
        });
        log_debug << "REST API deleted, count: " << count;
        return count;
    }

    long ApiGatewayMemoryRepository::deleteRestApi(const std::string &restApiId) const {
        boost::mutex::scoped_lock lock(_restApiMutex);

        const auto count = std::erase_if(_restApis, [restApiId](const auto &item) {
            auto const &[key, value] = item;
            return value.id == restApiId;
        });
        log_debug << "REST API deleted, count: " << count;
        return count;
    }

    // ========================================================================================================================
    // Usage plan
    // ========================================================================================================================
    bool ApiGatewayMemoryRepository::usagePlanExists(const std::string &id) const {
        return std::ranges::find_if(_usagePlans,
                                    [id](const std::pair<std::string, Entity::ApiGateway::UsagePlan> &p) {
                                        return p.second.id == id;
                                    }) != _usagePlans.end();
    }

    Entity::ApiGateway::UsagePlan ApiGatewayMemoryRepository::createUsagePlan(Entity::ApiGateway::UsagePlan &usagePlan) const {
        boost::mutex::scoped_lock lock(_usagePlanMutex);
        usagePlan.oid = Core::StringUtils::CreateRandomUuid();
        _usagePlans[usagePlan.oid] = usagePlan;
        return _usagePlans[usagePlan.oid];
    }

    Entity::ApiGateway::UsagePlan ApiGatewayMemoryRepository::getUsagePlanById(const std::string &id) const {
        boost::mutex::scoped_lock lock(_usagePlanMutex);
        const auto it = std::ranges::find_if(_usagePlans,
                                              [id](const std::pair<std::string, Entity::ApiGateway::UsagePlan> &p) {
                                                  return p.second.id == id;
                                              });
        if (it == _usagePlans.end()) throw Core::DatabaseException("Usage plan not found, id: " + id);
        return it->second;
    }

    std::vector<Entity::ApiGateway::UsagePlan> ApiGatewayMemoryRepository::listUsagePlans() const {
        boost::mutex::scoped_lock lock(_usagePlanMutex);
        std::vector<Entity::ApiGateway::UsagePlan> result;
        result.reserve(_usagePlans.size());
        for (const auto &[k, v]: _usagePlans) result.push_back(v);
        return result;
    }

    std::vector<Entity::ApiGateway::UsagePlan> ApiGatewayMemoryRepository::listUsagePlanCounters(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        boost::mutex::scoped_lock lock(_usagePlanMutex);

        auto q = Core::from(Core::NumberUtils::toVector(_usagePlans));
        if (!sortColumns.empty()) {
            for (const auto &sc: sortColumns) {
                if (sc.column == "id") {
                    q = q.order_by([](const Entity::ApiGateway::UsagePlan &a, const Entity::ApiGateway::UsagePlan &b) { return a.id < b.id; });
                }
                if (sc.column == "name") {
                    q = q.order_by([](const Entity::ApiGateway::UsagePlan &a, const Entity::ApiGateway::UsagePlan &b) { return a.name < b.name; });
                }
            }
        }

        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::ApiGateway::UsagePlan &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        const auto resultVector = q.to_vector();
        return Core::PageVector(resultVector, pageSize, pageIndex);
    }

    long ApiGatewayMemoryRepository::countUsagePlans(const std::string &region, const std::string &prefix) const {
        auto q = Core::from(Core::NumberUtils::toVector(_usagePlans));

        if (!region.empty()) {
            q = q.where([region](const Entity::ApiGateway::UsagePlan &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::ApiGateway::UsagePlan &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        return static_cast<long>(q.count());
    }

    void ApiGatewayMemoryRepository::deleteUsagePlan(const std::string &id) const {
        boost::mutex::scoped_lock lock(_usagePlanMutex);
        std::erase_if(_usagePlans, [id](const auto &item) { return item.second.id == id; });
    }

    Entity::ApiGateway::UsagePlan ApiGatewayMemoryRepository::upsertUsagePlan(Entity::ApiGateway::UsagePlan &usagePlan) const {
        boost::mutex::scoped_lock lock(_usagePlanMutex);

        const auto it = std::ranges::find_if(_usagePlans,
                                             [&usagePlan](const std::pair<const std::string, Entity::ApiGateway::UsagePlan> &entry) {
                                                 return entry.second.region == usagePlan.region && entry.second.id == usagePlan.id;
                                             });
        if (it != _usagePlans.end()) {
            usagePlan.oid = it->first;
            usagePlan.modified = system_clock::now();
            _usagePlans[it->first] = usagePlan;
            log_trace << "Usage plan updated, oid: " << it->first;
            return _usagePlans[it->first];
        }

        usagePlan.oid = Core::StringUtils::CreateRandomUuid();
        usagePlan.created = system_clock::now();
        _usagePlans[usagePlan.oid] = usagePlan;
        log_trace << "Usage plan created, oid: " << usagePlan.oid;
        return _usagePlans[usagePlan.oid];
    }

    void ApiGatewayMemoryRepository::importUsagePlan(Entity::ApiGateway::UsagePlan &usagePlan) const {
        usagePlan = upsertUsagePlan(usagePlan);
    }

    long ApiGatewayMemoryRepository::deleteAllUsagePlans() const {
        boost::mutex::scoped_lock lock(_usagePlanMutex);
        const long count = static_cast<long>(_usagePlans.size());
        _usagePlans.clear();
        return count;
    }

}// namespace Awsmock::Database
