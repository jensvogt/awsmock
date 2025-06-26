//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_DOCKER_SERVICE_H
#define AWSMOCK_SERVICE_DOCKER_SERVICE_H

// C++ standard includes
#include <string>
#include <thread>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/DomainSocketResult.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/RandomUtils.h>
#include <awsmock/core/StreamFilter.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/UnixSocket.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/docker/CreateContainerRequest.h>
#include <awsmock/dto/docker/CreateContainerResponse.h>
#include <awsmock/dto/docker/CreateNetworkRequest.h>
#include <awsmock/dto/docker/CreateNetworkResponse.h>
#include <awsmock/dto/docker/InspectContainerResponse.h>
#include <awsmock/dto/docker/ListContainerResponse.h>
#include <awsmock/dto/docker/ListImageResponse.h>
#include <awsmock/dto/docker/ListNetworkResponse.h>
#include <awsmock/dto/docker/PruneContainerResponse.h>
#include <awsmock/dto/docker/VersionResponse.h>

#ifdef _WIN32
#include <awsmock/core/WindowsSocket.h>
#endif

#define HOST_PORT_MIN 32768
#define HOST_PORT_MAX 65536

namespace AwsMock::Service {

    namespace http = boost::beast::http;
    using namespace std::chrono_literals;

    /**
     * @brief Controls the connection to the container daemon using a UNIX Domain socket.
     *
     * @par
     * All docker related commands will be executed by the different methods. This means create, delete, starting, stopping the docker images for DynamoDB, Lambdas etc. Supported
     * are docker container and podman container. Set the appropriate active flag in the configuration file.
     *
     * In order to run docker container use:
     * @code{.yaml}
     * awsmock:
     * ...
     *   docker:
     *     active: true
     * ...
     * @endcode
     *
     * To run it with podman
     * @code{.yaml}
     * awsmock:
     * ...
     *   podman:
     *     active: true
     * ...
     * @endcode
     *
     * @par Linux
     * On Linux, the service is using the docker REST API available at the UNIX domain socket. Depending on your Linux distribution, the docker socket is located under
     * different directory normally its: <i>/var/run/docker.sock</i> (Debian, Ubuntu). If your Linux distribution is using another location, set the full path to the
     * docker socket in the awsmock configuration file.
     *
     * @par Windows
     * On Windows, the HTTP port will be used to communicate with the docker daemon. Usually `http://localhost:2375`, if the port is not accessible, make the port accessible
     * in Docker Desktop for Windows.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ContainerService {

      public:

        /**
         * @brief Constructor
         */
        explicit ContainerService();

        /**
         * @brief Singleton instance
         */
        static ContainerService &instance() {
            static ContainerService dockerService;
            return dockerService;
        }

        /**
         * @brief Creates a simple image
         *
         * @param name image name
         * @param tag image tags
         * @param imageCode code of the image
         */
        void CreateImage(const std::string &name, const std::string &tag, const std::string &imageCode) const;

        /**
         * @brief Checks whether an image exists.
         *
         * @param name image name
         * @param tag image tags
         * @return true if image exists, otherwise false
         */
        [[nodiscard]] bool ImageExists(const std::string &name, const std::string &tag) const;

        /**
         * @brief Returns a image by name/tags.
         *
         * @param name container name
         * @param tag container tags
         * @param locked if true is already locked
         * @return Image
         */
        [[nodiscard]] Dto::Docker::Image GetImageByName(const std::string &name, const std::string &tag, bool locked = false) const;

        /**
         * @brief Build a docker image for a lambda
         *
         * @param codeDir code directory
         * @param name lambda function name, used as image name
         * @param tag
         * @param handler lambda function handler
         * @param runtime lambda AWS runtime
         * @param environment runtime environment
         * @return file size in bytes
         */
        [[nodiscard]] std::string BuildImage(const std::string &codeDir, const std::string &name, const std::string &tag, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment) const;

        /**
         * @brief Build a docker image from a docker file
         *
         * @param name lambda function name, used as image name
         * @param tag image tags
         * @param dockerFile docker file
         * @return file size in bytes
         */
        [[nodiscard]] std::string BuildImage(const std::string &name, const std::string &tag, const std::string &dockerFile) const;

        /**
         * @brief Delete an image by name/tags.
         *
         * @param id image ID
         */
        void DeleteImage(const std::string &id) const;

        /**
         * @brief Checks whether a container exists.
         *
         * @param name container name
         * @param tag container tags
         * @return true if container exists, otherwise false
         */
        [[nodiscard]] bool ContainerExists(const std::string &name, const std::string &tag) const;

        /**
         * @brief Checks whether a container exists by ID.
         *
         * @param id container ID
         * @return true if the container exists, otherwise false
         */
        [[nodiscard]] bool ContainerExists(const std::string &id) const;

        /**
         * @brief Checks whether a container exists by name.
         *
         * @param containerName container name
         * @return true if the container exists, otherwise false
         */
        [[nodiscard]] bool ContainerExistsByName(const std::string &containerName) const;

        /**
         * @brief Checks whether a container exists by image name.
         *
         * @par
         * The image name should be given als '<image>:<tag>'.
         *
         * @param imageName image name including the tag
         * @param tag image tag
         * @return true if the container exists, otherwise false
         */
        [[nodiscard]] bool ContainerExistsByImageName(const std::string &imageName, const std::string &tag) const;

        /**
         * @brief Checks whether a container is in running state
         *
         * @par
         * Sends an inspect container request to the docker daemon REST api.
         *
         * @param containerId container ID
         * @return true if the container is running, otherwise false
         */
        [[nodiscard]] bool IsContainerRunning(const std::string &containerId) const;

        /**
         * @brief Waits until a container is in state 'running'
         *
         * @par
         * The maximal waiting time can  be configured in the configuration file. Configuration key: 'awsmock.docker.container.maxWaitTime'.
         *
         * @param containerId container ID
         */
        void WaitForContainer(const std::string &containerId) const;

        /**
         * @brief List all docker images, with the given name and different tags.
         *
         * @param name name of the image
         * @return ListImageResponse
         */
        [[nodiscard]] Dto::Docker::ListImageResponse ListImages(const std::string &name) const;

        /**
         * @brief Creates a container
         *
         * @par
         * Supported standard values for the network mode are: bridge, host, none, and container:<name|id>. Any other value is taken as a custom network's name to which this container should connect to.
         *
         * @param imageName image name
         * @param instanceName name of the instance
         * @param tag image tags
         * @param environment runtime environment variables
         * @param hostPort external port of the lambda
         * @return CreateContainerResponse
         */
        [[nodiscard]] Dto::Docker::CreateContainerResponse CreateContainer(const std::string &imageName, const std::string &instanceName, const std::string &tag, const std::vector<std::string> &environment, int hostPort) const;

        /**
         * @brief Creates a container for a predefined image.
         *
         * @par
         * Supported standard values for the network mode are: bridge, host, none, and container:<name|id>. Any other value is taken as a custom network's name to which this container should connect to.
         *
         * @param imageName image name
         * @param tag image tags
         * @param containerName name of the container
         * @param hostPort external port of the container
         * @param containerPort internal port of the container
         * @return CreateContainerResponse
         */
        [[nodiscard]] Dto::Docker::CreateContainerResponse CreateContainer(const std::string &imageName, const std::string &tag, const std::string &containerName, int hostPort, int containerPort) const;

        /**
         * @brief Returns a container by name/tags.
         *
         * @param name container name
         * @param tag container tags
         * @return Container
         */
        [[nodiscard]] Dto::Docker::Container GetFirstContainerByImageName(const std::string &name, const std::string &tag) const;

        /**
         * @brief Returns a container by ID.
         *
         * @param containerId container ID
         * @return Container
         */
        [[nodiscard]] Dto::Docker::Container GetContainerById(const std::string &containerId) const;

        /**
         * @brief Inspect a container
         *
         * @param containerId container ID
         * @return Container
         * @see Dto::Docker::InspectContainerResponse
         */
        [[nodiscard]] Dto::Docker::InspectContainerResponse InspectContainer(const std::string &containerId) const;

        /**
         * @brief Returns a container by name.
         *
         * @param name container name
         * @return Container
         */
        [[nodiscard]] Dto::Docker::Container GetContainerByName(const std::string &name) const;

        /**
         * @brief List all containers of a given image.
         *
         * @param name image name
         * @param tag image tag
         * @return list of containers
         */
        [[nodiscard]] std::vector<Dto::Docker::Container> ListContainerByImageName(const std::string &name, const std::string &tag) const;

        /**
         * @brief Checks whether a network exists.
         *
         * @param name network name
         * @return true if the network exists, otherwise false
         */
        [[nodiscard]] bool NetworkExists(const std::string &name) const;

        /**
         * @brief Creates a network with the given name
         *
         * @param request create a network request
         * @return CreateNetworkResponse
         */
        [[nodiscard]] Dto::Docker::CreateNetworkResponse CreateNetwork(const Dto::Docker::CreateNetworkRequest &request) const;

        /**
         * @brief Creates a network with the given name
         *
         * @param containerId ID of the container
         * @param start timestamp
         * @return log messages as string
         */
        [[nodiscard]] std::string GetContainerLogs(const std::string &containerId, const system_clock::time_point &start) const;

        /**
         * @brief Start the container
         *
         * @param containerId container ID
         * @param containerName name of the container
         */
        void StartDockerContainer(const std::string &containerId, const std::string &containerName) const;

        /**
         * @brief Restart the container
         *
         * @param containerId container ID
         */
        void RestartDockerContainer(const std::string &containerId) const;

        /**
         * @brief Restart the container
         *
         * @param container container DTO
         */
        void RestartContainer(const Dto::Docker::Container &container) const;

        /**
         * @brief Stops the container
         *
         * @param container container DTO
         */
        void StopContainer(const Dto::Docker::Container &container) const;

        /**
         * @brief Stops the container by ID
         *
         * @param containerId container ID
         */
        void StopContainer(const std::string &containerId) const;

        /**
         * @brief Deletes the container
         *
         * @param container container DTO
         */
        void DeleteContainer(const Dto::Docker::Container &container) const;

        /**
         * @brief Delete all containers with the given image name as ancestor
         *
         * @param imageName image name
         * @param tag image tag
         */
        void DeleteContainers(const std::string &imageName, const std::string &tag) const;

        /**
         * @brief Deletes the container by ID
         *
         * @param containerId container ID
         */
        void DeleteContainer(const std::string &containerId) const;

        /**
         * @brief Deletes all stopped containers.
         */
        void PruneContainers() const;

      private:

        /**
         * @brief Write the docker file.
         *
         * @param codeDir code directory
         * @param handler handler function
         * @param runtime docker image runtime
         * @param environment runtime environment
         * @return return docker file path
         */
        static std::string WriteDockerFile(const std::string &codeDir, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment);

        /**
         * @brief Write the compressed docker image file.
         *
         * @param codeDir code directory
         * @param name function name
         * @return return docker file path
         */
        static std::string BuildImageFile(const std::string &codeDir, const std::string &name);

        /**
         * @brief Returns the network name
         *
         * @par
         * Depending on the container engine, the network name must be different. For Podman use the default name 'podman', for docker
         * we use an own network, named 'local'.
         *
         * @return network name
         */
        static std::string GetNetworkName();

        /**
         * @brief Extracts the handler file name from the handler name.
         *
         * @param handler handler
         * @return name of the handler file to copy to docker file
         */
        static std::string GetHandlerFileNodeJs22(const std::string &handler);

        /**
         * @brief Add environment variables to the Dockerfile
         *
         * @param ofs output strean
         * @param environment environment variables
         */
        static void AddEnvironment(std::ofstream &ofs, const std::map<std::string, std::string> &environment);

        /**
         * Docker version
         */
        std::string _dockerVersion;

        /**
         * Docker internal network name
         */
        std::string _networkName;

        /**
         * Docker internal network name
         */
        std::string _containerPort;

        /**
         * Using docker: true, else: false: use podman
         */
        bool _isDocker = false;

        /**
         * Docker listening socket
         */
        std::string _containerSocketPath;

        /**
         * Docker listening socket
         */
        std::shared_ptr<Core::DomainSocket> _domainSocket;

        /**
         * Mutex
         */
        static boost::mutex _dockerServiceMutex;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_DOCKER_SERVICE_H
