//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/service/container/ContainerService.h>

namespace AwsMock::Service {

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

        const std::string filters = Core::StringUtils::UrlEncode(R"({"reference":[")" + name + ":" + tag + "\"]}");
        if (const auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/images/json?all=true&filters=" + filters, {}, {}); statusCode == http::status::ok) {
            Dto::Docker::ListImageResponse response;
            response.FromJson(body);
            if (response.imageList.empty()) {
                log_debug << "Docker image not found, name: " << name << ":" << tag;
                return false;
            }
            log_debug << "Docker image found, name: " << name << ":" << tag;
            return true;
        }
        log_error << "Image exists request failed";
        return false;
    }

    void ContainerService::CreateImage(const std::string &name, const std::string &tag, const std::string &fromImage) const {

        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/images/create?name=" + name + "&tag=" + tag + "&fromImage=" + fromImage); statusCode == http::status::ok) {
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

        const std::string imageName = name + ":" + tag;
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/images/" + imageName + "/json");
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

    std::string ContainerService::BuildLambdaImage(const std::string &codeDir, const std::string &name, const std::string &tag, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment) const {
        log_debug << "Build image request, name: " << name << " tags: " << tag << " runtime: " << runtime;

        std::string dockerFile = WriteLambdaDockerFile(codeDir, handler, runtime, environment);
        std::string imageFile = BuildImageFile(codeDir, name);
        auto [statusCode, body, contentLength] = _domainSocket->SendBinary(http::verb::post, "/build?t=" + name + ":" + tag, imageFile);
        log_trace << "Build image, status: " << statusCode << ", body: " << body;
        if (statusCode != http::status::ok) {
            log_error << "Build image failed, statusCode: " << statusCode << " body: " << body;
            return {};
        }

        log_debug << "Build image request finished, name: " << name << " tags: " << tag << " runtime: " << runtime;
        return imageFile;
    }

    std::string ContainerService::BuildApplicationImage(const std::string &codeDir, const std::string &name, const std::string &tag, const std::string &runtime, const std::string &archive, const long privatePort, const std::map<std::string, std::string> &environment) const {
        log_debug << "Build image request, name: " << name << " tags: " << tag;

        // Write the docker file
        std::string dockerFile = WriteApplicationDockerFile(codeDir, archive, privatePort, runtime, environment);
        const std::string imageFile = BuildImageFile(codeDir, name);
        if (auto [statusCode, body, contentLength] = _domainSocket->SendBinary(http::verb::post, "/build?t=" + name + ":" + tag, imageFile, {}); statusCode != http::status::ok) {
            log_error << "Build image failed, statusCode: " << statusCode << " body: " << body;
        }
        return dockerFile;
    }

    std::string ContainerService::BuildDynamoDbImage(const std::string &name, const std::string &tag, const std::string &dockerFile) const {
        log_debug << "Build image request, name: " << name << " tags: " << tag;

        // Write the docker file
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

        if (auto [statusCode, body, contentLength] = _domainSocket->SendBinary(http::verb::post, "/build?t=" + name + ":" + tag, imageFile, {}); statusCode != http::status::ok) {
            log_error << "Build image failed, statusCode: " << statusCode << " body: " << body;
        }
        return dockerFile;
    }

    Dto::Docker::ListImageResponse ContainerService::ListImages(const std::string &name) const {

        Dto::Docker::ListImageResponse response{};
        const std::string filters = Core::StringUtils::UrlEncode(R"({"reference":[")" + name + "\"]}");
        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/images/json?all=true&filters=" + filters); statusCode == http::status::ok) {
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

        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::delete_, "/images/" + id + "?force=true"); statusCode != http::status::ok) {
            log_error << "Delete image failed, statusCode: " << statusCode << ", id: " << id;
        }
        log_debug << "Image deleted, id: " << id;
    }

    bool ContainerService::ContainerExists(const std::string &id) const {

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"id":[")" + id + "\"]}");
            if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters); statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, id: " << id;
                return !response.containerList.empty();
            } else {
                log_warning << "Docker container exists failed, statusCode: " << statusCode;
                return false;
            }
        } else {
            if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + id + "/exists"); statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, id: " << id;
                return !response.containerList.empty();
            } else {
                log_warning << "Docker container exists failed, statusCode: " << statusCode;
                return false;
            }
        }
    }

    bool ContainerService::ContainerExists(const std::string &name, const std::string &tag) const {

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + name + ":" + tag + "\"]}");
            auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, name: " << name << ":" << tag;
                return !response.containerList.empty();
            }
            log_warning << "Docker container exists failed, statusCode: " << statusCode;
            return false;
        }
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + name + "/exists");
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

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"name":[")" + containerName + "\"]}");
            auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, name: " << containerName;
                return !response.containerList.empty();
            }
            log_warning << "Docker container exists failed, statusCode: " << statusCode;
            return false;
        }
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + containerName + "/exists");
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

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + imageName + ":" + tag + "\"]}");
            auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode == http::status::ok) {
                const Dto::Docker::ListContainerResponse response(body);
                log_debug << "Docker container found, name: " << imageName;
                return !response.containerList.empty();
            }
            log_info << "Docker container by image failed, statusCode: " << statusCode;
            return false;
        }
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/containers/" + imageName + "/exists");
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

        const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + name + ":" + tag + "\"]}");
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&size=true&filters=" + filters);
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

        const std::string filters = Core::StringUtils::UrlEncode(R"({"id":[")" + containerId + "\"]}");
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
        if (statusCode != http::status::ok) {
            log_warning << "Get docker container by ID failed, statusCode: " << statusCode;
            return {};
        }

        Dto::Docker::ListContainerResponse response(body);
        if (response.containerList.empty()) {
            log_info << "Docker container not found, id: " << containerId;
            return {};
        }

        if (response.containerList.size() > 1) {
            log_warning << "More than one docker container found, id: " << containerId << " count: " << response.containerList.size();
        }

        log_debug << "Docker container found, id: " << containerId;
        return response.containerList.front();
    }

    Dto::Docker::Container ContainerService::GetContainerByName(const std::string &name) const {

        const std::string filters = Core::StringUtils::UrlEncode(R"({"name":[")" + name + "\"]}");
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&size=true&filters=" + filters);
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

        Dto::Docker::InspectContainerResponse inspectContainerResponse{};
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/json?size=true");
        inspectContainerResponse.status = statusCode;
        if (statusCode != http::status::ok) {
            log_warning << "Inspect container failed, state: " << statusCode << ", containerId: " << containerId;
            return inspectContainerResponse;
        }

        inspectContainerResponse.FromJson(body);
        log_debug << "Container found, containerId: " << containerId;
        return inspectContainerResponse;
    }

    std::vector<Dto::Docker::Container> ContainerService::ListContainerByImageName(const std::string &name, const std::string &tag) const {

        if (_isDocker) {
            const std::string filters = Core::StringUtils::UrlEncode(R"({"ancestor":[")" + name + ":" + tag + "\"]}");
            auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
            if (statusCode != http::status::ok) {
                log_warning << "Get docker container by name failed, state: " << statusCode << ", name: " << name << ":" << tag;
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
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/json?all=true&filters=" + filters);
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

        // Create the request
        const Dto::Docker::CreateContainerRequest request = {
                .hostName = instanceName,
                .image = imageName + ":" + tag,
                .networkMode = GetNetworkName(),
                .environment = environment,
                .containerPort = _containerPort,
                .hostPort = std::to_string(hostPort)};

        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/create?name=" + instanceName, request.ToJson());
        if (statusCode != http::status::created) {
            log_info << "Create container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return {};
        }

        Dto::Docker::CreateContainerResponse response = {.hostPort = hostPort};
        response.FromJson(body);

        log_debug << "Docker container created, name: " << imageName << ":" << tag << " id: " << response.id;
        return response;
    }

    Dto::Docker::CreateContainerResponse ContainerService::CreateContainer(const std::string &imageName, const std::string &instanceName, const std::string &tag, const std::vector<std::string> &environment, const int hostPort, const int containerPort) const {

        // Create the request
        const Dto::Docker::CreateContainerRequest request = {
                .hostName = instanceName,
                .image = imageName + ":" + tag,
                .networkMode = GetNetworkName(),
                .environment = environment,
                .containerPort = std::to_string(containerPort),
                .hostPort = std::to_string(hostPort)};

        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/create?name=" + instanceName, request.ToJson());
        if (statusCode != http::status::created) {
            log_info << "Create container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return {};
        }

        Dto::Docker::CreateContainerResponse response = {.hostPort = hostPort};
        response.FromJson(body);

        log_debug << "Docker container created, name: " << imageName << ":" << tag << " id: " << response.id;
        return response;
    }

    Dto::Docker::CreateContainerResponse ContainerService::CreateContainer(const std::string &imageName, const std::string &tag, const std::string &containerName, const int hostPort, const int containerPort) const {

        // Create the request
        const Dto::Docker::CreateContainerRequest request = {
                .hostName = imageName,
                .image = imageName + ":" + tag,
                .networkMode = GetNetworkName(),
                .containerPort = std::to_string(containerPort),
                .hostPort = std::to_string(hostPort)};
        const std::string jsonBody = request.ToJson();

        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/create?name=" + containerName, jsonBody);
        if (statusCode != http::status::created) {
            log_warning << "Create container failed, statusCode: " << statusCode << " body " << body;
            return {};
        }

        Dto::Docker::CreateContainerResponse response = {.hostPort = hostPort};
        response.FromJson(body);

        log_debug << "Docker container created, name: " << imageName << ":" << tag;
        return response;
    }

    void ContainerService::ContainerAttach(const std::string &containerId, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws, long tail) const {

        if (containerId.empty()) {
            log_error << "Empty container Id";
            return;
        }

        ws.control_callback([&ws](const boost::beast::websocket::frame_type kind, const boost::beast::string_view message) {
            if (kind == boost::beast::websocket::frame_type::close) {
                ws.close(boost::beast::websocket::close_code::abnormal);
            } else if (kind == boost::beast::websocket::frame_type::ping) {
                const boost::beast::websocket::ping_data pd(message);
                ws.pong(boost::beast::websocket::ping_data("message"));
            }
        });

        // First the last 1000 lines
        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/logs?tail=" + std::to_string(tail) + "&stdout=true&stderr=true"); statusCode == http::status::ok && contentLength > 0) {
            ws.text(true);
            ws.write(boost::asio::buffer(Core::StringUtils::RemoveColorCoding(body)));
        }

        try {
            boost::asio::streambuf buffer;
            system_clock::time_point last = system_clock::now();
            while (ws.is_open()) {
                const std::string since = std::to_string(Core::DateTimeUtils::UnixTimestamp(last));
                if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/logs?since=" + since + "&stdout=true&stderr=true&tail=1000"); statusCode == http::status::ok && contentLength > 0) {
                    if (ws.is_open()) {
                        ws.text(true);
                        ws.write(boost::asio::buffer(Core::StringUtils::RemoveColorCoding(body)));
                    }
                }
                last = system_clock::now();

                // Check for a closing message
                ws.read(buffer);
                log_trace << "Container read, message: " << boost::beast::make_printable(buffer.data());
                if (const Dto::Apps::WebSocketCommand webSocketCommand = Dto::Apps::WebSocketCommand::FromJson(boost::beast::buffers_to_string(buffer.data()));
                    webSocketCommand.command == Dto::Apps::WebSoketCommandType::CLOSE_LOG || webSocketCommand.command == Dto::Apps::WebSoketCommandType::UNKNOWN) {
                    ws.close({"Graceful shutdown"});
                    log_info << "Container logging connection closed, containerId: " << containerId;
                    break;
                }
                buffer.consume(buffer.size());
            }
        } catch (boost::exception &e) {
            log_info << "Websocket killed, containerId: " << containerId;
        }
        log_info << "Attached to container finished, containerId: " << containerId;
    }

    bool ContainerService::NetworkExists(const std::string &name) const {
        const std::string filters = Core::StringUtils::UrlEncode(R"({"name":[")" + name + "\"]}");

        if (_isDocker) {
            auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/networks/?filters=" + filters);
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
            auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/v5.0.0/libpod/networks/" + name + "/exists");
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

        Dto::Docker::CreateNetworkResponse response;
        if (_isDocker) {

            if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/networks/create", request.ToJson()); statusCode == http::status::ok) {
                log_debug << "Docker network created, name: " << request.name << " driver: " << request.driver;
                response.FromJson(body);
            } else {
                log_error << "Docker network create failed, statusCode: " << statusCode;
            }
        } else {

            if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/networks/create", request.ToJson()); statusCode == http::status::ok) {
                log_debug << "Podman network created, name: " << request.name << " driver: " << request.driver;
                response.FromJson(body);
            } else {
                log_error << "Podman network create failed, statusCode: " << statusCode;
            }
        }
        return response;
    }

    void ContainerService::StartDockerContainer(const std::string &containerId, const std::string &containerName) const {
        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/" + containerId + "/start"); statusCode != http::status::ok && statusCode != http::status::no_content) {
            log_warning << "Start container failed, id: " << containerName << ", statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container started, name: " << containerName << ", id: " << containerId;
    }

    bool ContainerService::IsContainerRunning(const std::string &containerId) const {

        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/json"); statusCode == http::status::ok) {
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

    void ContainerService::RestartDockerContainer(const std::string &containerId) const {
        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/" + containerId + "/restart"); statusCode != http::status::no_content) {
            log_warning << "Restart container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container restarted, containerId: " << containerId;
    }

    std::string ContainerService::GetContainerLogs(const std::string &containerId, const system_clock::time_point &start) const {

        std::string logMessages;
        const std::string since = std::to_string(Core::DateTimeUtils::UnixTimestamp(start));
        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/logs?since=" + since + "&stdout=true&stderr=true");
        if (statusCode == http::status::ok) {
            log_debug << "Container logs received, containerId: " << containerId;
            return body;
        }
        log_error << "Receive container logs failed, containerId: " << containerId << ", statusCode: " << statusCode;
        return {};
    }

    Dto::Docker::ContainerStat ContainerService::GetContainerStats(const std::string &containerId) const {

        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::get, "/containers/" + containerId + "/stats?stream=false");
        if (statusCode == http::status::ok) {
            log_debug << "Container statistics received, containerId: " << containerId;
            return Dto::Docker::ContainerStat::FromJson(body);
        }
        log_error << "Get container stats failed, containerId: " << containerId << ", statusCode: " << statusCode;
        return {};
    }

    void ContainerService::StopContainer(const Dto::Docker::Container &container) const {
        StopContainer(container.id);
    }

    void ContainerService::StopContainer(const std::string &containerId) const {

        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/" + containerId + "/stop"); statusCode != http::status::no_content && statusCode != http::status::not_modified) {
            log_warning << "Stop container failed, statusCode: " << statusCode;
            return;
        }
        log_debug << "Docker container stopped, id: " << containerId;
    }

    void ContainerService::KillContainer(const Dto::Docker::Container &container, const std::string &signal) const {
        KillContainer(container.id, signal);
    }

    void ContainerService::KillContainer(const std::string &containerId, const std::string &signal) const {

        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/" + containerId + "/kill?signal=" + signal); statusCode != http::status::no_content) {
            log_warning << "Kill container failed, statusCode: " << statusCode;
            return;
        }
        log_debug << "Docker container killed, id: " << containerId << ", signal: " << signal;
    }

    void ContainerService::DeleteContainer(const Dto::Docker::Container &container) const {
        DeleteContainer(container.id);
    }

    void ContainerService::DeleteContainer(const std::string &containerId) const {

        if (auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::delete_, "/containers/" + containerId + "?force=true"); statusCode != http::status::no_content) {
            log_warning << "Delete container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container deleted, id: " << containerId;
    }

    void ContainerService::DeleteContainers(const std::string &imageName, const std::string &tag) const {
        for (const std::vector<Dto::Docker::Container> containers = ListContainerByImageName(imageName, tag); const auto &container: containers) {
            DeleteContainer(container.id);
        }
        log_debug << "All docker containers deleted, id: " << imageName << ":" << tag;
    }

    void ContainerService::PruneContainers() const {

        auto [statusCode, body, contentLength] = _domainSocket->SendJson(http::verb::post, "/containers/prune");
        if (statusCode != http::status::ok) {
            log_warning << "Prune containers failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }

        Dto::Docker::PruneContainerResponse response;
        response.FromJson(body);

        log_debug << "Prune containers, count: " << response.containersDeleted.size() << " spaceReclaimed: " << response.spaceReclaimed;
    }

    std::string ContainerService::WriteLambdaDockerFile(const std::string &codeDir, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment) {

        std::string dockerFilename = codeDir + Core::FileUtils::separator() + "Dockerfile";
        std::string providedRuntime = boost::algorithm::to_lower_copy(runtime);
        Core::StringUtils::Replace(providedRuntime, ".", "-");
        auto supportedRuntime = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.runtime." + providedRuntime);
        auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        log_debug << "Using supported runtime, runtime: " << supportedRuntime;

        std::string awsConfig = codeDir + Core::FileUtils::separator() + "config";
        std::ofstream awsOfs(awsConfig);
        awsOfs << "[default]" << std::endl;
        awsOfs << "region=" << region << std::endl;
        awsOfs << "output=json" << std::endl;
        awsOfs.close();

        std::string awsCredentials = codeDir + Core::FileUtils::separator() + "credentials";
        std::ofstream awsCredOfs(awsCredentials);
        awsCredOfs << "[default]" << std::endl;
        awsCredOfs << "region=" << region << std::endl;
        awsCredOfs << "aws_access_key_id=none" << std::endl;
        awsCredOfs << "aws_secret_access_key=none" << std::endl;
        awsCredOfs << "aws_session_token=none" << std::endl;
        awsCredOfs << "retry_mode=standard" << std::endl;
        awsCredOfs << "max_attempts=1" << std::endl;
        awsCredOfs.close();

        std::ofstream ofs(dockerFilename);
        if (Core::StringUtils::StartsWithIgnoringCase(runtime, "java")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "RUN mkdir -p /root/.aws" << std::endl;
            ofs << "COPY config /root/.aws/" << std::endl;
            ofs << "COPY credentials /root/.aws/" << std::endl;
            ofs << "COPY classes ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "CMD [ \"" + handler + "::handleRequest\" ]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "postgres")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "CMD [ \"" + handler + "\" ]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "provided")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "COPY config /root/.aws/" << std::endl;
            ofs << "COPY credentials /root/.aws/" << std::endl;
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
            AddEnvironment(ofs, environment);
            ofs << "COPY requirements.txt ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "RUN pip install -r requirements.txt" << std::endl;
            ofs << "RUN mkdir -p /root/.aws" << std::endl;
            ofs << "COPY config /root/.aws/" << std::endl;
            ofs << "COPY credentials /root/.aws/" << std::endl;
            ofs << "COPY *.py ${LAMBDA_TASK_ROOT}/" << std::endl;
            ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "nodejs22")) {
            std::string handlerFile = GetHandlerFileNodeJs22(handler);
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/dist" << std::endl;
            ofs << "COPY node_modules/ ${LAMBDA_TASK_ROOT}/node_modules/" << std::endl;
            ofs << "COPY " << handlerFile << " ${LAMBDA_TASK_ROOT}/dist" << std::endl;
            ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "nodejs")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "COPY node_modules/ ${LAMBDA_TASK_ROOT}/node_modules/" << std::endl;
            ofs << "COPY index.js ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "go")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "COPY bootstrap ${LAMBDA_RUNTIME_DIR}" << std::endl;
            ofs << "RUN chmod 755 ${LAMBDA_RUNTIME_DIR}/bootstrap" << std::endl;
            ofs << "CMD [\"" + handler + "\"]" << std::endl;
        }
        ofs.close();
        log_debug << "Dockerfile written, filename: " << dockerFilename;

        return dockerFilename;
    }

    std::string ContainerService::WriteApplicationDockerFile(const std::string &codeDir, const std::string &archive, long privatePort, const std::string &runtime, const std::map<std::string, std::string> &environment) {

        std::string dockerFilename = codeDir + Core::FileUtils::separator() + "Dockerfile";
        std::string providedRuntime = boost::algorithm::to_lower_copy(runtime);
        Core::StringUtils::Replace(providedRuntime, ".", "-");
        auto supportedRuntime = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.runtime." + providedRuntime);
        auto region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
        log_debug << "Using supported runtime, runtime: " << supportedRuntime;

        std::string awsConfig = codeDir + Core::FileUtils::separator() + "config";
        std::ofstream awsOfs(awsConfig);
        awsOfs << "[default]" << std::endl;
        awsOfs << "region=" << region << std::endl;
        awsOfs << "output=json" << std::endl;
        awsOfs.close();

        std::string awsCredentials = codeDir + Core::FileUtils::separator() + "credentials";
        std::ofstream awsCredOfs(awsCredentials);
        awsCredOfs << "[default]" << std::endl;
        awsCredOfs << "region=" << region << std::endl;
        awsCredOfs << "aws_access_key_id=none" << std::endl;
        awsCredOfs << "aws_secret_access_key=none" << std::endl;
        awsCredOfs << "aws_session_token=none" << std::endl;
        awsCredOfs << "retry_mode=standard" << std::endl;
        awsCredOfs << "max_attempts=1" << std::endl;
        awsCredOfs.close();

        std::ofstream ofs(dockerFilename);
        if (Core::StringUtils::StartsWithIgnoringCase(runtime, "java")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "WORKDIR /app" << std::endl;
            ofs << "COPY " << archive << " app.jar" << std::endl;
            ofs << "RUN mkdir -p /root/.aws" << std::endl;
            ofs << "COPY config /root/.aws/" << std::endl;
            ofs << "COPY credentials /root/.aws/" << std::endl;
            ofs << "EXPOSE " << privatePort << std::endl;
            ofs << "ENTRYPOINT [\"java\", \"-jar\", \"app.jar\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "provided")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "COPY bootstrap ${LAMBDA_RUNTIME_DIR}" << std::endl;
            ofs << "RUN chmod 775 ${LAMBDA_RUNTIME_DIR}/bootstrap" << std::endl;
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/lib" << std::endl;
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/bin" << std::endl;
            ofs << "COPY bin/* ${LAMBDA_TASK_ROOT}/bin/" << std::endl;
            ofs << "COPY lib/* ${LAMBDA_TASK_ROOT}/lib/" << std::endl;
            ofs << "RUN chmod 775 -R ${LAMBDA_TASK_ROOT}/lib" << std::endl;
            ofs << "RUN chmod 775 -R ${LAMBDA_TASK_ROOT}/bin" << std::endl;
            //ofs << "CMD [ \"" + handler + "\" ]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "python")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "COPY requirements.txt ${LAMBDA_TASK_ROOT}" << std::endl;
            ofs << "RUN pip install -r requirements.txt" << std::endl;
            ofs << "RUN mkdir -p /root/.aws" << std::endl;
            ofs << "COPY config /root/.aws/" << std::endl;
            ofs << "COPY credentials /root/.aws/" << std::endl;
            ofs << "COPY *.py ${LAMBDA_TASK_ROOT}/" << std::endl;
            //ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "nodejs22")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/dist" << std::endl;
            ofs << "COPY node_modules/ ${LAMBDA_TASK_ROOT}/node_modules/" << std::endl;
            //ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "nodejs")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "COPY node_modules/ ${LAMBDA_TASK_ROOT}/node_modules/" << std::endl;
            ofs << "COPY index.js ${LAMBDA_TASK_ROOT}" << std::endl;
            //ofs << "CMD [\"" + handler + "\"]" << std::endl;
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "go")) {
            ofs << "FROM " << supportedRuntime << std::endl;
            AddEnvironment(ofs, environment);
            ofs << "COPY bootstrap ${LAMBDA_RUNTIME_DIR}" << std::endl;
            ofs << "RUN chmod 755 ${LAMBDA_RUNTIME_DIR}/bootstrap" << std::endl;
            //ofs << "CMD [\"" + handler + "\"]" << std::endl;
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

    std::string ContainerService::GetHandlerFileNodeJs22(const std::string &handler) {
        const std::string prefix = handler.substr(0, handler.find('.'));
        return prefix + ".mjs";
    }

    void ContainerService::AddEnvironment(std::ofstream &ofs, const std::map<std::string, std::string> &environment) {
        for (const auto &[fst, snd]: environment) {
            ofs << "ENV " << fst << "=\"" << snd << "\"" << std::endl;
        }
        ofs << "ENV " << "AWS_REGION=\"eu-central-1\"" << std::endl;
        ofs << "ENV " << "AWS_ACCESS_KEY_ID=\"none\"" << std::endl;
        ofs << "ENV " << "AWS_SECRET_ACCESS_KEY=\"none\"" << std::endl;
        ofs << "ENV " << "AWS_SESSION_TOKEN=\"none\"" << std::endl;
    }
}// namespace AwsMock::Service
