//
// Created by vogje01 on 1/5/25.
//

#include "TestBase.h"

#include <awsmock/core/TestUtils.h>

namespace AwsMock::Core {
    /**
     * Initialize database
     */
    bool TestBase::InitUnitTests() {

        TestUtils::CreateTestConfigurationFile();
        LogStream::Initialize();
        LogStream::RemoveConsoleLogs();

        InitializeShm();

        return true;
    }

    void TestBase::InitializeShm() {

        // As Awsmock is not running under root set shared memory permissions
        //boost::interprocess::permissions unrestricted_permissions;
        //unrestricted_permissions.set_unrestricted();

        // Create a managed shared memory segment.
        //boost::interprocess::shared_memory_object::remove(MONITORING_SEGMENT_NAME);
        //shm = std::make_unique<boost::interprocess::managed_shared_memory>(boost::interprocess::open_or_create, MONITORING_SEGMENT_NAME, 65000, nullptr, unrestricted_permissions);
    }

}// namespace AwsMock::Core

// entry point:
int main(const int argc, char *argv[]) {

    return boost::unit_test::unit_test_main(&AwsMock::Core::TestBase::InitUnitTests, argc, argv);
}
