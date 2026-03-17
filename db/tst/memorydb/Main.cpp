//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_MODULE DatabaseTests
#include <boost/test/included/unit_test.hpp>

// Awsmock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/repository/DynamoDbDatabase.h>

struct GlobalTestFixture {

    GlobalTestFixture() {

        // Initialize logging
        AwsMock::Core::LogStream::Initialize();
        AwsMock::Core::LogStream::RemoveConsoleLogs();

        // Create test configuration
        AwsMock::Core::TestUtils::CreateTestConfigurationFile(false);
    }

    ~GlobalTestFixture() {
        const long itemCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllItems();
        log_debug << "Items deleted, count: " << itemCount;
        const long tableCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllTables();
        log_debug << "Tables deleted, count: " << tableCount;
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
