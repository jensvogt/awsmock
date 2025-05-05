//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/service/container/ContainerService.h>

namespace AwsMock::Service {
    std::map<std::string, std::string> ContainerService::_supportedRuntimes = {
            {"java11", "public.ecr.aws/lambda/java:11"},
            {"java17", "public.ecr.aws/lambda/java:17"},
            {"java21", "public.ecr.aws/lambda/java:21.2024.11.22.15"},
            {"python3.8", "public.ecr.aws/lambda/python:3.8"},
            {"python3.9", "public.ecr.aws/lambda/python:3.9"},
            {"python3.10", "public.ecr.aws/lambda/python:3.10"},
            {"python3.11", "public.ecr.aws/lambda/python:3.11"},
            {"python3.12", "public.ecr.aws/lambda/python:3.12"},
            {"nodejs20.x", "public.ecr.aws/lambda/nodejs:20"},
            {"provided.al2", "public.ecr.aws/lambda/provided:al2"},
            {"provided.al2023", "public.ecr.aws/lambda/provided:al2023"},
            {"provided.latest", "public.ecr.aws/lambda/provided:latest"},
            {"go", "public.ecr.aws/lambda/provided:al2023"},
    };

    boost::mutex ContainerService::_dockerServiceMutex;

    ContainerService::ContainerService() {
        // Get network mode
        _networkName = Core::Configuration::instance().GetValue<std::string>("awsmock.docker.network-name");
        _containerPort = Core::Configuration::instance().GetValue<std::string>("awsmock.docker.container.port");
        _isDocker = Core::Configuration::instance().GetValue<bool>("awsmock.docker.active");
#ifdef WIN32
        _containerSocketPath = Core::Configuration::instance().GetValue<std::string>("awsmock.docker.socket");
        _domainSocket = std::make_shared<Core::WindowsSocket>(_containerSocketPath);
#else
        _containerSocketPath = _isDocker ? Core::Configuration::instance().GetValue<std::string>("awsmock.docker.socket") : Core::Configuration::instance().GetValue<std::string>("awsmock.podman.socket");
        _domainSocket = std::make_shared<Core::UnixSocket>(_containerSocketPath);
#endif
    }

    bool ContainerService::ImageExists(const std::string &name, const std::string &tag) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"reference":[")" + name + ":" + tag + "\"]}");
            if (const auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/images/json?all=true&filters=" + filters, {}, {}); statusCode == http::status::ok) {
                Dto::Docker::ListImageResponse response;
                response.FromJson(body);
                if (response.imageList.empty()) {
                    log_debug << "Docker image not found, name: " << name << ":" << tag;
                } else {
                    log_debug << "Docker image found, name: " << name << ":" << tag;
                }
                return !response.imageList.empty();
            }
        } else {
            if (const auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/images/" + name + "/exists");
                statusCode == http::status::no_content) {
                log_debug << "Podman image found, name: " << name << ":" << tag;
                return true;
            }
        }
        log_error << "Image exists request failed";
        return false;
    }

    void ContainerService::CreateImage(const std::string &name, const std::string &tag, const std::string &fromImage) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/images/create?name=" + name + "&tag=" + tag + "&fromImage=" + fromImage); statusCode == http::status::ok) {
            log_debug << "Docker image created, name: " << name << ":" << tag;

            // Wait for image creation
            Dto::Docker::Image image = GetImageByName(name, tag, true);
            while (GetImageByName(name, tag, true).size == 0) {
                std::this_thread::sleep_for(500ms);
            }
        } else {
            log_error << "Docker image create failed, statusCode: " << statusCode;
        }
    }

    Dto::Docker::Image ContainerService::GetImageByName(const std::string &name, const std::string &tag, const bool locked) const {
        if (!locked)
            boost::mutex::scoped_lock lock(_dockerServiceMutex);

        const std::string imageName = name + ":" + tag;
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/images/" + imageName + "/json");
        if (statusCode != http::status::ok) {
            log_warning << "Get image by name failed, name: " << imageName << ", statusCode: " << statusCode;
            return {};
        }
        Dto::Docker::Image response;
        response.FromJson(body);
        response.id = Core::StringUtils::Split(response.id, ':')[1];

        log_debug << "Image found, name: " << imageName;
        return response;
    }

    std::string ContainerService::BuildImage(const std::string &codeDir, const std::string &name, const std::string &tag, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);
        log_debug << "Build image request, name: " << name << " tags: " << tag << " runtime: " << runtime;

        std::string dockerFile = WriteDockerFile(codeDir, handler, runtime, environment);
        std::string imageFile = BuildImageFile(codeDir, name);

        if (auto [statusCode, body] = _domainSocket->SendBinary(http::verb::post, "/build?t=" + name + ":" + tag, imageFile); statusCode != http::status::ok) {
            log_error << "Build image failed, statusCode: " << statusCode << " body: " << body;
        }
        log_debug << "Build image request finished, name: " << name << " tags: " << tag << " runtime: " << runtime;
        return imageFile;
    }

    std::string ContainerService::BuildImage(const std::string &name, const std::string &tag, const std::string &dockerFile) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);
        log_debug << "Build image request, name: " << name << " tags: " << tag;

        // Write docker file
        const std::string codeDir = Core::DirUtils::CreateTempDir();
        const std::string fileName = codeDir + Core::FileUtils::separator() + "Dockerfile";
        std::ofstream ofs(fileName);
        ofs << dockerFile;
        ofs.close();

        // Create TAR file name
        std::string tarFileName = name;
        Core::StringUtils::Replace(tarFileName, "/", "-");
        Core::StringUtils::Replace(tarFileName, ".", "-");
        const std::string imageFile = BuildImageFile(codeDir, tarFileName);

        if (auto [statusCode, body] = _domainSocket->SendBinary(http::verb::post, "/build?t=" + name + ":" + tag, imageFile, {}); statusCode != http::status::ok) {
            log_error << "Build image failed, statusCode: " << statusCode << " body: " << body;
        }
        return dockerFile;
    }

    Dto::Docker::ListImageResponse ContainerService::ListImages(const std::string &name) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        Dto::Docker::ListImageResponse response{};
        const std::string filters = Core::StringUtils::UrlEncode(R"({"reference":[")" + name + "\"]}");
        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/images/json?all=true&filters=" + filters); statusCode == http::status::ok) {
            response.FromJson(body);
            if (response.imageList.empty()) {
                log_warning << "Docker image not found, name: " << name;
                return response;
            }

            if (response.imageList.size() > 1) {
                log_warning << "More than one docker image found, name: " << name;
                return response;
            }
            log_info << "Images found, name: " << name;
            return response;
        } else {
            log_error << "Get image by name failed, statusCode: " << statusCode;
        }
        return response;
    }

    void ContainerService::DeleteImage(const std::string &id) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::delete_, "/images/" + id + "?force=true"); statusCode != http::status::ok) {
            log_error << "Delete image failed, statusCode: " << statusCode << ", id: " << id;
        }
        log_debug << "Image deleted, id: " << id;
    }

    bool ContainerService::ContainerExists(const std::string &id) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"id":[")" + id + "\"]}");
            if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters); statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, id: " << id;
                return !response.containerList.empty();
            } else {
                log_warning << "Docker container exists failed, statusCode: " << statusCode;
                return false;
            }
        } else {
            if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + id + "/exists"); statusCode == http::status::ok) {
                Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, id: " << id;
                return !response.containerList.empty();
            } else {
                log_warning << "Docker container exists failed, statusCode: " << statusCode;
                return false;
            }
        }
    }

    bool ContainerService::ContainerExists(const std::string &name, const std::string &tag) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + name + ":" + tag + "\"]}");
            auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, name: " << name << ":" << tag;
                return !response.containerList.empty();
            }
            log_warning << "Docker container exists failed, statusCode: " << statusCode;
            return false;
        }
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + name + "/exists");
        if (statusCode == http::status::no_content) {
            log_debug << "Podman container found, name: " << name << ":" << tag;
            return true;
        }
        if (statusCode == http::status::not_found) {
            log_info << "Podman container not found";
        } else {
            log_error << "Podman container exists request failed, statusCode: " << statusCode;
        }
        return false;
    }

    bool ContainerService::ContainerExistsByName(const std::string &containerName) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"name":[")" + containerName + "\"]}");
            auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, name: " << containerName;
                return !response.containerList.empty();
            }
            log_warning << "Docker container exists failed, statusCode: " << statusCode;
            return false;
        }
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + containerName + "/exists");
        if (statusCode == http::status::no_content) {
            log_debug << "Podman container found, name: " << containerName;
            return true;
        }
        if (statusCode == http::status::not_found) {
            log_info << "Podman container not found";
        } else {
            log_error << "Podman container exists request failed, statusCode: " << statusCode;
        }
        return false;
    }

    bool ContainerService::ContainerExistsByImageName(const std::string &imageName, const std::string &tag) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + imageName + ":" + tag + "\"]}");
            auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, name: " << imageName;
                return !response.containerList.empty();
            }
            log_info << "Docker container by image failed, statusCode: " << statusCode;
            return false;
        }
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + imageName + "/exists");
        if (statusCode == http::status::no_content) {
            log_debug << "Podman container found, name: " << imageName;
            return true;
        }
        if (statusCode == http::status::not_found) {
            log_info << "Podman container not found";
        } else {
            log_error << "Podman container exists request failed, statusCode: " << statusCode;
        }
        return false;
    }

    Dto::Docker::Container ContainerService::GetFirstContainerByImageName(const std::string &name, const std::string &tag) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + name + ":" + tag + "\"]}");
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&size=true&filters=" + filters);
        if (statusCode != http::status::ok) {
            log_warning << "Get docker container by name failed, state: " << statusCode;
            return {};
        }

        Dto::Docker::ListContainerResponse response(body);
        if (response.containerList.empty()) {
            log_warning << "Docker container not found, name: " << name << ":" << tag;
            return {};
        }

        if (response.containerList.size() > 1) {
            log_warning << "More than one docker container found, name: " << name << ":" << tag << " count: " << response.containerList.size();
        }

        log_debug << "Docker container found, name: " << name << ":" << tag;
        return response.containerList.front();
    }

    Dto::Docker::Container ContainerService::GetContainerById(const std::string &containerId) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        const std::string filters = Core::StringUtils::UrlEncode(R"({"id":[")" + containerId + "\"]}");
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
        if (statusCode != http::status::ok) {
            log_warning << "Get docker container by name failed, statusCode: " << statusCode;
            return {};
        }

        Dto::Docker::ListContainerResponse response(body);
        if (response.containerList.empty()) {
            log_warning << "Docker container not found, id: " << containerId;
            return {};
        }

        if (response.containerList.size() > 1) {
            log_warning << "More than one docker container found, id: " << containerId << " count: " << response.containerList.size();
        }

        log_debug << "Docker container found, id: " << containerId;
        return response.containerList.front();
    }

    Dto::Docker::Container ContainerService::GetContainerByName(const std::string &name) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        const std::string filters = Core::StringUtils::UrlEncode(R"({"name":[")" + name + "\"]}");
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&size=true&filters=" + filters);
        if (statusCode != http::status::ok) {
            log_warning << "Get container by name failed, statusCode: " << statusCode;
            return {};
        }

        Dto::Docker::ListContainerResponse response(body);
        if (response.containerList.empty()) {
            log_warning << "Container not found, name: " << name;
            return {};
        }

        if (response.containerList.size() > 1) {
            log_warning << "More than one container found, name: " << name << " count: " << response.containerList.size();
        }

        log_debug << "Container found, name: " << name;
        return response.containerList.front();
    }

    Dto::Docker::InspectContainerResponse ContainerService::InspectContainer(const std::string &containerId) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        Dto::Docker::InspectContainerResponse inspectContainerResponse = {};
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/json");
        if (statusCode != http::status::ok) {
            log_warning << "Get container by name failed, state: " << statusCode;
            return inspectContainerResponse;
        }

        inspectContainerResponse.FromJson(body);
        log_debug << "Container found, containerId: " << containerId;
        return inspectContainerResponse;
    }

    std::vector<Dto::Docker::Container> ContainerService::ListContainerByImageName(const std::string &name, const std::string &tag) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + name + ":" + tag + "\"]}");
            auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode != http::status::ok) {
                log_warning << "Get docker container by name failed, state: " << statusCode;
                return {};
            }

            Dto::Docker::ListContainerResponse response(body);
            if (response.containerList.empty()) {
                log_info << "Docker container not found, name: " << name << ":" << tag;
                return {};
            }
            log_debug << "Docker container found, name: " << name << ":" << tag << " count: " << response.containerList.size();
            return response.containerList;
        }
        const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + name + ":" + tag + "\"]}");
        auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
        if (statusCode != http::status::ok) {
            log_warning << "Get docker container by name failed, state: " << statusCode;
            return {};
        }

        Dto::Docker::ListContainerResponse response(body);
        if (response.containerList.empty()) {
            log_info << "Docker container not found, name: " << name << ":" << tag;
            return {};
        }
        log_debug << "Docker container found, name: " << name << ":" << tag << " count: " << response.containerList.size();
        return response.containerList;
    }

    Dto::Docker::CreateContainerResponse ContainerService::CreateContainer(const std::string &imageName, const std::string &instanceName, const std::string &tag, const std::vector<std::string> &environment, const int hostPort) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        // Create the request
        const Dto::Docker::CreateContainerRequest request = {
                .hostName = instanceName,
                .image = imageName + ":" + tag,
                .networkMode = GetNetworkName(),
                .environment = environment,
                .containerPort = _containerPort,
                .hostPort = std::to_string(hostPort)};

        auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/containers/create?name=" + instanceName, request.ToJson());
        if (statusCode != http::status::created) {
            log_warning << "Create container failed, statusCode: " << statusCode << " body: " << body;
            return {};
        }

        Dto::Docker::CreateContainerResponse response = {.hostPort = hostPort};
        response.FromJson(body);

        log_debug << "Docker container created, name: " << imageName << ":" << tag << " id: " << response.id;
        return response;
    }

    Dto::Docker::CreateContainerResponse ContainerService::CreateContainer(const std::string &imageName, const std::string &tag, const std::string &containerName, const int hostPort, const int containerPort) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        // Create the request
        const Dto::Docker::CreateContainerRequest request = {
                .hostName = imageName,
                .image = imageName + ":" + tag,
                .networkMode = GetNetworkName(),
                .containerPort = std::to_string(containerPort),
                .hostPort = std::to_string(hostPort)};
        const std::string jsonBody = request.ToJson();

        auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/containers/create?name=" + containerName, jsonBody);
        if (statusCode != http::status::created) {
            log_warning << "Create container failed, statusCode: " << statusCode << " body " << body;
            return {};
        }

        Dto::Docker::CreateContainerResponse response = {.hostPort = hostPort};
        response.FromJson(body);

        log_debug << "Docker container created, name: " << imageName << ":" << tag;
        return response;
    }

    bool ContainerService::NetworkExists(const std::string &name) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);
        const std::string filters = Core::StringUtils::UrlEncode(R"({"name":[")" + name + "\"]}");

        if (_isDocker) {
            auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/networks/?filters=" + filters);
            if (statusCode == http::status::ok) {
                Dto::Docker::ListNetworkResponse response;
                response.FromJson(body);
                if (response.networkList.empty()) {
                    log_debug << "Docker network not found, name: " << name;
                } else {
                    log_debug << "Docker network found, name: " << name;
                }
                return !response.networkList.empty();
            }
            log_error << "Network exists request failed, statusCode: " << statusCode;
        } else {
            auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/networks/" + name + "/exists");
            if (statusCode == http::status::no_content) {
                log_debug << "Podman network found, name: " << name;
                return true;
            }
            if (statusCode == http::status::not_found) {
                log_info << "Podman network not found";
            } else {
                log_error << "Podman network exists request failed, statusCode: " << statusCode;
            }
        }
        return false;
    }

    Dto::Docker::CreateNetworkResponse ContainerService::CreateNetwork(const Dto::Docker::CreateNetworkRequest &request) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        Dto::Docker::CreateNetworkResponse response;
        if (_isDocker) {

            auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/networks/create", request.ToJson());
            if (statusCode == http::status::ok) {
                log_debug << "Docker network created, name: " << request.name << " driver: " << request.driver;
                response.FromJson(body);
            } else {
                log_error << "Docker network create failed, statusCode: " << statusCode;
            }
        } else {

            auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/networks/create", request.ToJson());
            if (statusCode == http::status::ok) {
                log_debug << "Podman network created, name: " << request.name << " driver: " << request.driver;
                response.FromJson(body);
            } else {
                log_error << "Podman network create failed, statusCode: " << statusCode;
            }
        }
        return response;
    }

    void ContainerService::StartDockerContainer(const std::string &id) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/containers/" + id + "/start"); statusCode != http::status::ok && statusCode != http::status::no_content) {
            log_warning << "Start container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container started, id: " << id;
    }

    bool ContainerService::IsContainerRunning(const std::string &containerId) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);
        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/json"); statusCode == http::status::ok) {
            log_debug << "Container running, statusCode: " << statusCode;
            Dto::Docker::InspectContainerResponse response;
            response.FromJson(body);
            log_debug << "Docker container state, id: " << containerId << " state: " << std::boolalpha << response.state.running;
            return response.state.running;
        }
        log_debug << "Is docker container running failed, id: " << containerId;
        return false;
    }

    void ContainerService::WaitForContainer(const std::string &containerId) const {
        const int checkTime = Core::Configuration::instance().GetValue<int>("awsmock.docker.container.checkTime");
        const int maxWaitTime = Core::Configuration::instance().GetValue<int>("awsmock.docker.container.maxWaitTime");
        const auto deadline = system_clock::now() + std::chrono::seconds{maxWaitTime};
        while (!IsContainerRunning(containerId) && system_clock::now() < deadline) {
            std::this_thread::sleep_for(std::chrono::milliseconds(checkTime));
        }
    }

    void ContainerService::RestartContainer(const Dto::Docker::Container &container) const {
        RestartDockerContainer(container.id);
    }

    void ContainerService::RestartDockerContainer(const std::string &id) const {
        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/containers/" + id + "/restart"); statusCode != http::status::no_content) {
            log_warning << "Restart container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container restarted, id: " << id;
    }

    void ContainerService::StopContainer(const Dto::Docker::Container &container) const {
        StopContainer(container.id);
    }

    void ContainerService::StopContainer(const std::string &id) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/containers/" + id + "/stop"); statusCode != http::status::no_content && statusCode != http::status::not_modified) {
            log_warning << "Stop container failed, statusCode: " << statusCode;
            return;
        }
        log_debug << "Docker container stopped, id: " << id;
    }

    void ContainerService::DeleteContainer(const Dto::Docker::Container &container) const {
        DeleteContainer(container.id);
    }

    void ContainerService::DeleteContainer(const std::string &containerId) const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        if (auto [statusCode, body] = _domainSocket->SendJson(http::verb::delete_, "/containers/" + containerId + "?force=true"); statusCode != http::status::no_content) {
            log_warning << "Delete container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container deleted, id: " << containerId;
    }

    void ContainerService::DeleteContainers(const std::string &imageName, const std::string &tag) const {
        for (std::vector<Dto::Docker::Container> containers = ListContainerByImageName(imageName, tag); const auto &container: containers) {
            DeleteContainer(container.id);
        }
        log_debug << "All docker containers deleted, id: " << imageName << ":" << tag;
    }

    void ContainerService::PruneContainers() const {
        boost::mutex::scoped_lock lock(_dockerServiceMutex);

        auto [statusCode, body] = _domainSocket->SendJson(http::verb::post, "/containers/prune");
        if (statusCode != http::status::ok) {
            log_warning << "Prune containers failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }

        Dto::Docker::PruneContainerResponse response;
        response.FromJson(body);

        log_debug << "Prune containers, count: " << response.containersDeleted.size() << " spaceReclaimed: " << response.spaceReclaimed;
    }

    std::string ContainerService::WriteDockerFile(const std::string &codeDir, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment) {

        std::string dockerFilename = codeDir + Core::FileUtils::separator() + "Dockerfile";
        std::string providedRuntime = boost::algorithm::to_lower_copy(runtime);
        Core::StringUtils::Replace(providedRuntime, ".", "-");
        auto supportedRuntime = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.runtime." + providedRuntime);
        log_debug << "Using supported runtime, runtime: " << supportedRuntime;

        std::ofstream ofs(dockerFilename);
        if (Core::StringUtils::StartsWithIgnoringCase(runtime, "java")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            for (const auto &[fst, snd]: environment) {
                ofs << "ENV " << fst << "=\"" << snd << "\"" << std::endl;
            }
            ofs << "COPY classes ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "CMD [ \"" + handler + "::handleRequest\" ]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "provided")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            for (const auto &[fst, snd]: environment) {
                ofs << "ENV " << fst << "=\"" << snd << "\"" << std::endl;
            }
            ofs << "COPY bootstrap ${LAMBDA_RUNTIME_DIR}" << std::endl;
            ofs << "RUN chmod 775 ${LAMBDA_RUNTIME_DIR}/bootstrap" << std::endl;
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/lib" << std::endl;
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/bin" << std::endl;
            ofs << "COPY bin/* ${LAMBDA_TASK_ROOT}/bin/" << std::endl;
            ofs << "COPY lib/* ${LAMBDA_TASK_ROOT}/lib/" << std::endl;
            ofs << "RUN chmod 775 -R ${LAMBDA_TASK_ROOT}/lib" << std::endl;
            ofs << "RUN chmod 775 -R ${LAMBDA_TASK_ROOT}/bin" << std::endl;
            ofs << "CMD [ \"" + handler + "\" ]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "python")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            for (const auto &[fst, snd]: environment) {
                ofs << "ENV " << fst << "=\"" << snd << "\"" << std::endl;
            }
            ofs << "COPY requirements.txt ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "RUN mkdir -p /root/.aws" << std::endl;
            ofs << "COPY config /root/.aws" << std::endl;
            ofs << "COPY credentials /root/.aws" << std::endl;
            ofs << "RUN pip install -r requirements.txt" << std::endl;
            ofs << "COPY lambda_function.py ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "nodejs")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            for (const auto &[fst, snd]: environment) {
                ofs << "ENV " << fst << "=\"" << snd << "\"" << std::endl;
            }
            ofs << "COPY node_modules/ ${LAMBDA_TASK_ROOT}/node_modules/" << std::endl;
            ofs << "COPY index.js ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "RUN mkdir -p /root/.aws" << std::endl;
            ofs << "COPY config /root/.aws" << std::endl;
            ofs << "COPY credentials /root/.aws" << std::endl;
            ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "go")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            for (const auto &[fst, snd]: environment) {
                ofs << "ENV " << fst << "=\"" << snd << "\"" << std::endl;
            }
            ofs << "COPY bootstrap ${LAMBDA_RUNTIME_DIR}" << std::endl;
            ofs << "RUN chmod 755 ${LAMBDA_RUNTIME_DIR}/bootstrap" << std::endl;
            ofs << "RUN mkdir -p /root/.aws" << std::endl;
            ofs << "COPY config /root/.aws" << std::endl;
            ofs << "COPY credentials /root/.aws" << std::endl;
            ofs << "CMD [\"" + handler + "\"]" << std::endl;
        }
        ofs.close();
        log_debug << "Dockerfile written, filename: " << dockerFilename;

        return dockerFilename;
    }

    std::string ContainerService::BuildImageFile(const std::string &codeDir, const std::string &name) {
        std::string tarFileName = codeDir + Core::FileUtils::separator() + name + ".tgz";
        Core::TarUtils::TarDirectory(tarFileName, codeDir + Core::FileUtils::separator());
        log_debug << "Zipped TAR file written: " << tarFileName;

        return tarFileName;
    }

    std::string ContainerService::GetNetworkName() {
        if (Core::Configuration::instance().GetValue<bool>("awsmock.dockerized")) {
            return Core::Configuration::instance().GetValue<std::string>("awsmock.docker.network-name");
        }
        return Core::Configuration::instance().GetValue<std::string>("awsmock.podman.network-name");
    }
}// namespace AwsMock::Service
