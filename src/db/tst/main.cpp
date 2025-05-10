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

// Boost includes
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/thread.hpp>

// AwsMock includes
#include "awsmock/core/SharedMemoryUtils.h"


#include <awsmock/core/TestUtils.h>
#include <awsmock/utils/ConnectionPool.h>
#include <boost/interprocess/managed_shared_memory.hpp>

class TestEnvironment final : public ::testing::Environment {
  public:

    // Initialise a test configuration.
    void SetUp() override {
        AwsMock::Core::TestUtils::CreateTestConfigurationFile();
        InitializeDatabase();
        InitializeShm();
    }

    /**
     * Initialize database
     */
    static void InitializeDatabase() {

        // Get database variables
        const auto name = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.name");
        const auto host = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.host");
        const auto user = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.user");
        const auto password = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.password");
        const int _port = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.port");
        const int poolSize = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(_port) + "/?maxPoolSize=" + std::to_string(poolSize));

        AwsMock::Database::ConnectionPool &pool = AwsMock::Database::ConnectionPool::instance();

        // Options
        pool.Configure();
    }

    void InitializeShm() {

        // As Awsmock is not running under root set shared memory permissions
        boost::interprocess::permissions unrestricted_permissions;
        unrestricted_permissions.set_unrestricted();

        // Create a managed shared memory segment.
        boost::interprocess::shared_memory_object::remove(SHARED_MEMORY_SEGMENT_NAME);
        shm = std::make_unique<boost::interprocess::managed_shared_memory>(boost::interprocess::open_or_create, SHARED_MEMORY_SEGMENT_NAME, 65000, nullptr, unrestricted_permissions);
    }

    /**
     * Global shared memory segment
     */
    std::unique_ptr<boost::interprocess::managed_shared_memory> shm;
};

int main(int argc, char **argv) {

    // Run data
    testing::InitGoogleTest(&argc, argv);
    AddGlobalTestEnvironment(new TestEnvironment);
    const int ret = RUN_ALL_TESTS();

    return ret;
}