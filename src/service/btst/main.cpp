//
// Created by vogje01 on 01/09/2022.
//

// C++ includes

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE ServiceTests

// Boost includes
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/thread.hpp>

// AwsMock includes
#include <awsmock/core/SharedMemoryUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/dto/docker/CreateContainerResponse.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/utils/ConnectionPool.h>
#include <awsmock/utils/TestUtils.h>

std::unique_ptr<boost::interprocess::managed_shared_memory> shm;
AwsMock::Database::ConnectionPool &pool = AwsMock::Database::ConnectionPool::instance();

#define TEST_IMAGE_NAME std::string("jensvogt/awsmock-test")
#define TEST_CONTAINER_VERSION std::string("latest")
#define TEST_CONTAINER_NAME std::string("awsmock-test")

#include "KMSServiceCliTests.cpp"
#include "KMSServiceJavaTests.cpp"
#include "KMSServiceTests.cpp"
#include "SQSServiceCliTests.cpp"
#include "SQSServiceJavaTests.cpp"
#include "SecretsManagerServiceTests.cpp"

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

bool init_unit_test() {

    AwsMock::Core::TestUtils::CreateTestConfigurationFile();
    AwsMock::Core::LogStream::Initialize();
    AwsMock::Core::LogStream::RemoveConsoleLogs();

    InitializeDatabase();
    InitializeShm();

    // Create services
    AwsMock::Database::TestUtils::CreateServices();

    // Create a docker service
    AwsMock::Service::ContainerService containerService = AwsMock::Service::ContainerService::instance();

    // Check image
    if (!containerService.ImageExists(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION)) {
        containerService.CreateImage(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION, TEST_IMAGE_NAME);
    }

    // Check container
    if (!containerService.ContainerExists(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION)) {
        AwsMock::Dto::Docker::CreateContainerResponse response = containerService.CreateContainer(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION, TEST_CONTAINER_NAME, 10100, 10100);
    }

    // Start the docker container, in case it is not already running.
    if (const AwsMock::Dto::Docker::Container container = containerService.GetFirstContainerByImageName(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION); container.state != "running") {
        containerService.StartDockerContainer(container.id);
        log_info << "Test docker container started";
    } else {
        log_info << "Test docker container already running";
    }

    // Initialize monitoring
    AwsMock::Monitoring::MetricService::instance().Initialize();
    return true;
}

// entry point:
int main(const int argc, char *argv[]) {

    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}