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

    Entity::ApiGateway::Key ApiGatewayMemoryDb::CreateKey(const Entity::ApiGateway::Key &key) {
        boost::mutex::scoped_lock lock(_apiGatewayMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _apiKeys[oid] = key;
        log_trace << "Application created, oid: " << oid;
        return _apiKeys[oid];
    }

}// namespace AwsMock::Database
