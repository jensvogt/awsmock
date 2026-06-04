//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_MODULE DatabaseTests
#include <boost/test/included/unit_test.hpp>

// Awsmock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/repository/RepositoryFactory.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

struct GlobalTestFixture {

    GlobalTestFixture() {

        // Initialize logging
        Awsmock::Core::LogStream::Initialize();
        Awsmock::Core::LogStream::RemoveConsoleLogs();

        // Create test configuration
        Awsmock::Core::TestUtils::CreateTestConfigurationFile(false);
    }

    ~GlobalTestFixture() {
        const long itemCount = Awsmock::Database::RepositoryFactory::instance().dynamodbRepository()->deleteAllItems();
        log_debug << "Items deleted, count: " << itemCount;
        const long tableCount = Awsmock::Database::RepositoryFactory::instance().dynamodbRepository()->deleteAllTables();
        log_debug << "Tables deleted, count: " << tableCount;
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
