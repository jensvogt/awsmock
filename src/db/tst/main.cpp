//
// Created by vogje01 on 01/09/2022.
//

// C++ includes
#include <memory>

// GTest includes
#include "gtest/gtest.h"

// MongoDB includes
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/utils/ConnectionPool.h>

class TestEnvironment final : public ::testing::Environment {
  public:

    // Initialise a test configuration.
    void SetUp() override {
        AwsMock::Core::TestUtils::CreateTestConfigurationFile();
        InitializeDatabase();
    }

    /**
     * Initialize database
     */
    static void InitializeDatabase() {

        // Get database variables
        const AwsMock::Core::Configuration &_configuration = AwsMock::Core::Configuration::instance();
        const std::string name = _configuration.GetValueString("awsmock.mongodb.name");
        const std::string host = _configuration.GetValueString("awsmock.mongodb.host");
        const std::string user = _configuration.GetValueString("awsmock.mongodb.user");
        const std::string password = _configuration.GetValueString("awsmock.mongodb.password");
        const int _port = _configuration.GetValueInt("awsmock.mongodb.port");
        const int poolSize = _configuration.GetValueInt("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(_port) + "/?maxPoolSize=" + std::to_string(poolSize));

        AwsMock::Database::ConnectionPool &pool = AwsMock::Database::ConnectionPool::instance();

        // Options
        pool.Configure();
    }
};

int main(int argc, char **argv) {

    // Run data
    testing::InitGoogleTest(&argc, argv);
    AddGlobalTestEnvironment(new TestEnvironment);
    const int ret = RUN_ALL_TESTS();

    return ret;
}