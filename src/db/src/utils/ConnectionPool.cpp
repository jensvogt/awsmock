//
// Created by vogje01 on 07/05/2024.
//

#include <awsmock/utils/ConnectionPool.h>

namespace AwsMock::Database {

    void ConnectionPool::Configure() {
        const Core::Configuration &configuration = Core::Configuration::instance();
        const std::string name = configuration.GetValueString("awsmock.mongodb.name");
        const std::string host = configuration.GetValueString("awsmock.mongodb.host");
        const std::string user = configuration.GetValueString("awsmock.mongodb.user");
        const std::string password = configuration.GetValueString("awsmock.mongodb.password");
        const int port = configuration.GetValueInt("awsmock.mongodb.port");
        const int poolSize = configuration.GetValueInt("awsmock.mongodb.pool-size");

        // MongoDB URL
        std::string tmp = "mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(port) + "/?maxPoolSize=" + std::to_string(poolSize);
        mongocxx::uri _uri(tmp);
        log_info << "Using MongoDB database url: " << _uri.to_string();

        // Create connection pool
        _instance = std::make_shared<mongocxx::instance>();
        _pool = std::make_shared<mongocxx::pool>(_uri);
        log_info << "MongoDB database initialized";
    }

    mongocxx::pool::entry ConnectionPool::GetConnection() const {
        return _pool->acquire();
    }

    void ConnectionPool::Shutdown() {
        _pool.reset();
    }

}// namespace AwsMock::Database
