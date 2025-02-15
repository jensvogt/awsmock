//
// Created by vogje01 on 01/09/2022.
//

// C++ include
#include <fstream>

// GTest includes
#include <gtest/gtest.h>

// Test includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/utils/TestUtils.h>

#include <boost/asio/io_context.hpp>
#include <boost/thread/detail/thread.hpp>

#define TEST_IMAGE_NAME std::string("jensvogt/awsmock-test")
#define TEST_CONTAINER_VERSION std::string("latest")
#define TEST_CONTAINER_NAME std::string("awsmock-test")

class TestEnvironment final : public ::testing::Environment {

  public:

    // Initialise a test configuration.
    void SetUp() override {

        AwsMock::Core::TestUtils::CreateTestConfigurationFile(false);
        AwsMock::Database::TestUtils::CreateServices();

        // Create docker service
        dockerService = AwsMock::Service::ContainerService::instance();

        // Check image
        if (!dockerService.ImageExists(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION)) {
            dockerService.CreateImage(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION, TEST_IMAGE_NAME);
        }

        // Check container
        if (!dockerService.ContainerExists(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION)) {
            AwsMock::Dto::Docker::CreateContainerResponse response = dockerService.CreateContainer(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION, TEST_CONTAINER_NAME, 10100, 10100);
        }

        // Get docker container

        // Start docker container, in case it is not already running.
        if (const AwsMock::Dto::Docker::Container container = dockerService.GetFirstContainerByImageName(TEST_IMAGE_NAME, TEST_CONTAINER_VERSION); container.state != "running") {
            dockerService.StartDockerContainer(container.id);
            log_info << "Test docker container started";
        } else {
            log_info << "Test docker container already running";
        }

        // Initialize monitoring
        AwsMock::Monitoring::MetricService::instance().Initialize();
    }

  private:

    /**
     * Docker service
     */
    AwsMock::Service::ContainerService dockerService;
};

int main(int argc, char **argv) {

    // Run data
    testing::InitGoogleTest(&argc, argv);
    AddGlobalTestEnvironment(new TestEnvironment);
    const int ret = RUN_ALL_TESTS();

    return ret;
}