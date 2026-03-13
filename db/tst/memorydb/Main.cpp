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
#include <awsmock/core/monitoring/MonitoringCollector.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/utils/ConnectionPool.h>

std::unique_ptr<boost::interprocess::managed_shared_memory> shm;

struct GlobalTestFixture {

    static void InitializeShm() {

        // As Awsmock is not running under root set shared memory permissions
        boost::interprocess::permissions unrestricted_permissions;
        unrestricted_permissions.set_unrestricted();

        // Create a managed shared memory segment.
        boost::interprocess::shared_memory_object::remove(MONITORING_SEGMENT_NAME);
        shm = std::make_unique<boost::interprocess::managed_shared_memory>(boost::interprocess::open_or_create, MONITORING_SEGMENT_NAME, 65000, nullptr, unrestricted_permissions);
    }

    GlobalTestFixture() {

        // Initialize logging
        AwsMock::Core::LogStream::Initialize();
        AwsMock::Core::LogStream::RemoveConsoleLogs();

        // Create test configuration
        AwsMock::Core::TestUtils::CreateTestConfigurationFile(false);

        // Initialize shared memory
        InitializeShm();
    }

    ~GlobalTestFixture() {
        const long itemCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllItems();
        log_debug << "Items deleted, count: " << itemCount;
        const long tableCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllTables();
        log_debug << "Tables deleted, count: " << tableCount;
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
