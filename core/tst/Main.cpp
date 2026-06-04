//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_MODULE CoreTests
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <boost/test/included/unit_test.hpp>

struct GlobalTestFixture {
    GlobalTestFixture() {
        Awsmock::Core::LogStream::Initialize();
        Awsmock::Core::LogStream::RemoveConsoleLogs();
        Awsmock::Core::TestUtils::CreateTestConfigurationFile(false);
    }

    ~GlobalTestFixture() {
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
