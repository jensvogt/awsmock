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
        const std::string name = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.name");
        const std::string host = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.host");
        const std::string user = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.user");
        const std::string password = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.password");
        const int _port = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.port");
        const int poolSize = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.pool-size");

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