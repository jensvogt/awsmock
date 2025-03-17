//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_CONTAINER_SERVICE_TEST_H
#define AWMOCK_CORE_CONTAINER_SERVICE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/service/container/ContainerService.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define HELLO_WORLD_IMAGE "hello-world"
#define HELLO_WORLD_TAG "latest"
#define K8S_PAUSE_IMAGE "k8s.gcr.io/pause"
#define K8S_PAUSE_TAG "latest"

namespace AwsMock::Service {

    class DockerServiceTest : public ::testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
            if (_service.ContainerExists(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG)) {
                for (const auto &container: _service.ListContainerByImageName(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG)) {
                    _service.StopContainer(container.id);
                    _service.DeleteContainer(container.id);
                }
            }
            if (_service.ContainerExists(K8S_PAUSE_IMAGE, K8S_PAUSE_TAG)) {
                for (const auto &container: _service.ListContainerByImageName(K8S_PAUSE_IMAGE, K8S_PAUSE_TAG)) {
                    _service.StopContainer(container.id);
                    _service.DeleteContainer(container.id);
                }
            }
        }

        Core::Configuration _configuration = Core::Configuration(TMP_PROPERTIES_FILE);
        ContainerService _service = ContainerService();
    };

    TEST_F(DockerServiceTest, ImageExistsTest) {

        // arrange
        _service.CreateImage(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG, HELLO_WORLD_IMAGE);

        // act
        const bool result = _service.ImageExists(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(DockerServiceTest, ContainerExistsTest) {

        // arrange
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        const std::vector<std::string> environment;
        _service.CreateImage(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG, HELLO_WORLD_IMAGE);
        const Dto::Docker::CreateContainerResponse response = _service.CreateContainer(HELLO_WORLD_IMAGE, instanceId, HELLO_WORLD_TAG, environment, 1025);
        EXPECT_FALSE(response.id.empty());

        // act
        const bool result = _service.ContainerExists(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(DockerServiceTest, ContainerStartTest) {

        // arrange
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        const std::vector<std::string> environment;
        _service.CreateImage(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG, HELLO_WORLD_IMAGE);
        const Dto::Docker::CreateContainerResponse response = _service.CreateContainer(HELLO_WORLD_IMAGE, instanceId, HELLO_WORLD_TAG, environment, 1025);
        EXPECT_FALSE(response.id.empty());
        const std::string containerId = response.id;
        _service.StartDockerContainer(containerId);

        // act
        Dto::Docker::Container result = _service.GetContainerById(containerId);

        // assert
        EXPECT_TRUE(result.state == "created" || result.state == "running");
    }

    TEST_F(DockerServiceTest, ContainerIsRunningTest) {

        // arrange
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        const std::vector<std::string> environment;
        _service.CreateImage(HELLO_WORLD_IMAGE, HELLO_WORLD_TAG, HELLO_WORLD_IMAGE);
        const Dto::Docker::CreateContainerResponse response = _service.CreateContainer(HELLO_WORLD_IMAGE, instanceId, HELLO_WORLD_TAG, environment, 1025);
        EXPECT_FALSE(response.id.empty());
        const std::string containerId = response.id;
        _service.StartDockerContainer(containerId);

        // act
        const bool result = _service.IsContainerRunning(containerId);

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(DockerServiceTest, ContainerWaitTest) {

        // arrange
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        const std::vector<std::string> environment;
        _service.CreateImage(K8S_PAUSE_IMAGE, K8S_PAUSE_TAG, K8S_PAUSE_IMAGE);
        const Dto::Docker::CreateContainerResponse response = _service.CreateContainer(K8S_PAUSE_IMAGE, instanceId, K8S_PAUSE_TAG, environment, 1025);
        EXPECT_FALSE(response.id.empty());
        const std::string containerId = response.id;
        _service.StartDockerContainer(containerId);
        _service.WaitForContainer(containerId);

        // act
        const bool result = _service.IsContainerRunning(containerId);

        // assert
        EXPECT_TRUE(result);
    }
}// namespace AwsMock::Service

#endif// AWMOCK_CORE_CONTAINER_SERVICE_TEST_H