//
// Created by vogje01 on 1/5/25.
//

// Boost includes

// Boost includes
#include <boost/interprocess/managed_shared_memory.hpp>

// Local includes
#include "./TestBase.h"

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/monitoring/MonitoringCollector.h>

namespace AwsMock::Dto {

    std::unique_ptr<boost::interprocess::managed_shared_memory> shm;

    /**
     * Initialize database
     */
    bool TestBase::InitUnitTests() {

        Core::TestUtils::CreateTestConfigurationFile();
        Core::LogStream::Initialize();
        Core::LogStream::RemoveConsoleLogs();

        // InitializeDatabase();
        InitializeShm();

        // Initialize monitoring
        // AwsMock::Monitoring::MetricService::instance().Initialize();
        return true;
    }

    void TestBase::InitializeShm() {

        // As Awsmock is not running under root set shared memory permissions
        boost::interprocess::permissions unrestricted_permissions;
        unrestricted_permissions.set_unrestricted();

        // Create a managed shared memory segment.
        boost::interprocess::shared_memory_object::remove(MONITORING_SEGMENT_NAME);
        shm = std::make_unique<boost::interprocess::managed_shared_memory>(boost::interprocess::open_or_create, MONITORING_SEGMENT_NAME, 65000, nullptr, unrestricted_permissions);
    }

}// namespace AwsMock::Dto

// entry point:
int main(const int argc, char *argv[]) {

    return boost::unit_test::unit_test_main(&AwsMock::Dto::TestBase::InitUnitTests, argc, argv);
}
