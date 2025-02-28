//
// Created by vogje01 on 07/05/2024.
//

#include <awsmock/utils/ConnectionPool.h>

namespace AwsMock::Database {

    void ConnectionPool::Configure() {
        Core::Configuration configuration = Core::Configuration::instance();
        const std::string name = configuration.GetValueString("awsmock.mongodb.name");
        const std::string host = configuration.GetValueString("awsmock.mongodb.host");
        const std::string user = configuration.GetValueString("awsmock.mongodb.user");
        const std::string password = configuration.GetValueString("awsmock.mongodb.password");
        const int port = configuration.GetValueInt("awsmock.mongodb.port");
        const int poolSize = configuration.GetValueInt("awsmock.mongodb.pool-size");

        // MongoDB URL
        const std::string url = "mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(port) + "/?maxPoolSize=" + std::to_string(poolSize);
        mongocxx::uri _uri(url);
        log_info << "Using MongoDB database url: " << url;

        // Create connection pool
        _instance = std::make_unique<mongocxx::instance>();
        _pool = std::make_unique<mongocxx::pool>(std::move(_uri));
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        log_info << "MongoDB database initialized, version: " << mongocxx::v_noabi::options::server_api::version_to_string(api.get_version());
    }

    mongocxx::pool::entry ConnectionPool::GetConnection() const {
        return _pool->acquire();
    }

    bsoncxx::stdx::optional<mongocxx::pool::entry> ConnectionPool::TryGetConnection() const {
        return _pool->try_acquire();
    }

    void ConnectionPool::Shutdown() {
        _pool.release();
    }

}// namespace AwsMock::Database
