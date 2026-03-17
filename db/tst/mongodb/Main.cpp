//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_MODULE DatabaseTests
#include <boost/test/included/unit_test.hpp>

// Boost includes
#include <boost/interprocess/permissions.hpp>

// Awsmock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/utils/ConnectionPool.h>

std::unique_ptr<boost::interprocess::managed_shared_memory> shm;

struct GlobalTestFixture {

    /**
     * Initialize database
     */
    static void InitializeDatabase() {

        // Get database variables
        const auto name = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.name");
        const auto host = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.host");
        const auto user = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.user");
        const auto password = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.password");
        const int port = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.port");
        const int poolSize = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(port) + "/?maxPoolSize=" + std::to_string(poolSize));

        // Options
        AwsMock::Database::ConnectionPool &_pool = AwsMock::Database::ConnectionPool::instance();
        _pool.Configure();
    }

    GlobalTestFixture() {

        // Initialize logging
        AwsMock::Core::LogStream::Initialize();
        AwsMock::Core::LogStream::RemoveConsoleLogs();

        // Create test configuration
        AwsMock::Core::TestUtils::CreateTestConfigurationFile(true);

        // Initialize database
        InitializeDatabase();
    }

    ~GlobalTestFixture() {
        const long itemCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllItems();
        log_debug << "Items deleted, count: " << itemCount;
        const long tableCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllTables();
        log_debug << "Tables deleted, count: " << tableCount;
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
