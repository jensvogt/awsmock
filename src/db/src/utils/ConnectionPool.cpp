//
// Created by vogje01 on 07/05/2024.
//

#include <awsmock/utils/ConnectionPool.h>

namespace AwsMock::Database {

    boost::mutex ConnectionPool::_mutex;

    void ConnectionPool::Configure() {

        const Core::Configuration &configuration = Core::Configuration::instance();
        const auto name = configuration.GetValue<std::string>("awsmock.mongodb.name");
        const auto host = configuration.GetValue<std::string>("awsmock.mongodb.host");
        const auto user = configuration.GetValue<std::string>("awsmock.mongodb.user");
        const auto password = configuration.GetValue<std::string>("awsmock.mongodb.password");
        const auto port = configuration.GetValue<std::string>("awsmock.mongodb.port");
        const auto poolSize = configuration.GetValue<std::string>("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + port + "/?maxPoolSize=" + poolSize);

        // Create a connection pool
        _instance = std::make_unique<mongocxx::instance>();
        _pool = std::make_unique<mongocxx::pool>(_uri);
        log_info << "MongoDB database initialized";
    }

    mongocxx::pool::entry ConnectionPool::GetConnection() const {
        boost::mutex::scoped_lock lock(_mutex);

        if (!_pool) {
            log_error << "Mongo database connection failure, error: pool not existing";
        }
        mongocxx::pool::entry connection = _pool->acquire();
        if (!connection) {
            log_error << "Mongo database connection failure, error: no connection available";
            throw Core::DatabaseException("No connection available");
        }
        return connection;
    }

    void ConnectionPool::Shutdown() {
        _pool.reset();
    }

}// namespace AwsMock::Database
