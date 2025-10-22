//
// Created by vogje01 on 07/05/2024.
//

#include <awsmock/utils/ConnectionPool.h>

namespace AwsMock::Database {

    void ConnectionPool::Configure() {
        const Core::Configuration &configuration = Core::Configuration::instance();
        const auto name = configuration.GetValue<std::string>("awsmock.mongodb.name");
        const auto host = configuration.GetValue<std::string>("awsmock.mongodb.host");
        const auto user = configuration.GetValue<std::string>("awsmock.mongodb.user");
        const auto password = configuration.GetValue<std::string>("awsmock.mongodb.password");
        const int port = configuration.GetValue<int>("awsmock.mongodb.port");
        const int poolSize = configuration.GetValue<int>("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(port) + "/?maxPoolSize=" + std::to_string(poolSize));

        // Create a connection pool
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
