//
// Created by vogje01 on 06/06/2023.
//

#include <awsmock/service/container/ContainerService.h>

namespace Awsmock::Service {

    thread_local std::shared_ptr<Core::DomainSocket> ContainerService::_domainSocket;

    ContainerService::ContainerService() {
        _networkName = GetNetworkName();
        _containerPort = Core::Configuration::instance().get<std::string>("awsmock.docker.container.port");

        _tlsEnabled = Core::Configuration::instance().get<bool>("awsmock.docker.tls.enabled");
        if (_tlsEnabled) {
            _tlsHost = Core::Configuration::instance().get<std::string>("awsmock.docker.tls.host");
            _tlsPort = Core::Configuration::instance().get<int>("awsmock.docker.tls.port");
            _tlsCaFile = Core::Configuration::instance().get<std::string>("awsmock.docker.tls.ca-cert");
            _tlsCertFile = Core::Configuration::instance().get<std::string>("awsmock.docker.tls.cert");
            _tlsKeyFile = Core::Configuration::instance().get<std::string>("awsmock.docker.tls.key");
            _tlsVerifyPeer = Core::Configuration::instance().get<bool>("awsmock.docker.tls.verify-peer");
            log_info << "Docker TLS enabled, host: " << _tlsHost << ", port: " << _tlsPort;
            _initialized = true;
        } else {
#ifdef WIN32
            _containerSocketPath = Core::Configuration::instance().get<std::string>("awsmock.docker.socket");
            _initialized = true;
#else
            _containerSocketPath = Core::Configuration::instance().get<std::string>("awsmock.docker.socket");
            if (!std::filesystem::exists(_containerSocketPath)) {
                log_warning << "Docker socket not found, path: " << _containerSocketPath << " - container operations will fail";
                _initialized = false;
            } else if (std::filesystem::status(_containerSocketPath).type() != std::filesystem::file_type::socket) {
                log_warning << "Docker socket path exists but is not a socket, path: " << _containerSocketPath << " - container operations will fail";
                _initialized = false;
            } else {
                _initialized = true;
            }
#endif
        }
    }

    std::shared_ptr<Core::DomainSocket> ContainerService::GetSocket() const {
        if (!_domainSocket) {
            if (_tlsEnabled) {
                _domainSocket = std::make_shared<Core::TlsSocket>(_tlsHost, _tlsPort, _tlsCaFile, _tlsCertFile, _tlsKeyFile, _tlsVerifyPeer);
            } else {
#ifdef WIN32
                _domainSocket = std::make_shared<Core::WindowsSocket>(_containerSocketPath);
#else
                _domainSocket = std::make_shared<Core::UnixSocket>(_containerSocketPath);
#endif
            }
        }
        return _domainSocket;
    }

    bool ContainerService::ImageExists(const std::string &name, const std::string &tag) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name << ":" << tag;
            return false;
        }
        const Core::DomainSocketResult result = GetSocket()->SendJson(http::verb::get, "/images/" + name + ":" + tag + "/json", {}, {});
        if (result.statusCode == http::status::ok) {
            log_debug << "Docker image found, name: " << name << ":" << tag;
            return true;
        }
        log_info << "Image does not exist, name: " << name << ":" << tag << ", httpStatus: " << result.statusCode;
        return false;
    }

    void ContainerService::CreateImage(const std::string &name, const std::string &tag, const std::string &fromImage) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name << ":" << tag;
            return;
        }
        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/images/create?name=" + name + "&tag=" + tag + "&fromImage=" + fromImage); statusCode == http::status::ok) {
            while (GetImageByName(name, tag, true).size == 0) {
                std::this_thread::sleep_for(500ms);
            }
            log_debug << "Docker image created, name: " << name << ":" << tag;
        } else {
            log_error << "Docker image create failed, name: " << name << ":" << tag << ", statusCode: " << statusCode;
        }
    }

    Dto::Docker::Image ContainerService::GetImageByName(const std::string &name, const std::string &tag, const bool locked) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name << ":" << tag;
            return {};
        }
        const std::string imageName = name + ":" + tag;
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/images/" + imageName + "/json");
        if (statusCode != http::status::ok) {
            log_warning << "Get image by name failed, name: " << imageName << ", statusCode: " << statusCode;
            return {};
        }
        Dto::Docker::Image response = Dto::Docker::Image::FromJson(body);
        response.id = Core::StringUtils::Split(response.id, ":")[1];
        log_debug << "Image found, name: " << imageName;
        return response;
    }

    std::string ContainerService::BuildLambdaImage(const std::string &codeDir, const std::string &name, const std::string &tag, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name << ":" << tag;
            return {};
        }
        log_debug << "Build image request, name: " << name << " tags: " << tag << " runtime: " << runtime;
        std::string dockerFile = WriteLambdaDockerFile(codeDir, handler, runtime, environment);
        std::string imageFile = BuildImageFile(codeDir, name);

        auto [statusCode, body, contentLength] = GetSocket()->SendBinary(http::verb::post, "/build?t=" + name + ":" + tag, imageFile);
        log_trace << "Build image, status: " << statusCode << ", body: " << body;
        if (statusCode != http::status::ok) {
            log_error << "Build image failed, statusCode: " << statusCode << " body: " << body;
            return {};
        }
        for (const auto &line: Core::StringUtils::SplitOnNewline(body)) {
            if (line.empty()) continue;
            boost::system::error_code ec;
            if (const boost::json::value jv = boost::json::parse(line, ec); !ec && jv.is_object()) {
                if (const auto *err = jv.as_object().if_contains("error")) {
                    log_error << "Build image failed, name: " << name << ":" << tag << " error: " << boost::json::serialize(*err);
                    return {};
                }
            }
        }
        log_info << "Build image request finished, name: " << name << " version: " << tag << " runtime: " << runtime;
        return imageFile;
    }

    std::string ContainerService::BuildApplicationImage(const std::string &codeDir, Database::Entity::Apps::Application &applicationEntity) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << applicationEntity.name << ":" << applicationEntity.version;
            return {};
        }
        log_debug << "Build image request, name: " << applicationEntity << ", tags: " << applicationEntity.version;
        std::string dockerFile = WriteApplicationDockerFile(codeDir, applicationEntity);
        const std::string imageFile = BuildImageFile(codeDir, applicationEntity.name);

        auto [statusCode, body, contentLength] = GetSocket()->SendBinary(http::verb::post, "/build?t=" + applicationEntity.name + ":" + applicationEntity.version, imageFile, {});
        if (statusCode != http::status::ok) {
            log_error << "Build image failed, image: " << applicationEntity.name << ":" << applicationEntity.version << ", statusCode: " << statusCode << ", body: " << body;
            return {};
        }
        for (const auto &line: Core::StringUtils::SplitOnNewline(body)) {
            if (line.empty()) continue;
            boost::system::error_code ec;
            if (const boost::json::value jv = boost::json::parse(line, ec); !ec && jv.is_object()) {
                if (const auto *err = jv.as_object().if_contains("error")) {
                    log_error << "Build image failed, name: " << applicationEntity.name << ":" << applicationEntity.version << " error: " << boost::json::serialize(*err);
                    return {};
                }
            }
        }
        log_info << "Build image successful, name: " << applicationEntity.name << ":" << applicationEntity.version;
        return dockerFile;
    }

    std::vector<Dto::Docker::Image> ContainerService::ListImagesByName(const std::string &name, const std::string &tag) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name << ":" << tag;
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/images/json?all=true");
        if (statusCode != http::status::ok) {
            log_error << "List images failed, statusCode: " << statusCode;
            throw Core::ServiceException("Get image by name failed", statusCode);
        }

        const Dto::Docker::ListImageResponse response = Dto::Docker::ListImageResponse::FromJson(body);
        if (response.imageList.empty()) {
            log_warning << "Docker image not found, name: " << name;
            return {};
        }

        const std::string target = tag.empty() ? name : name + ":" + tag;
        auto images = response.imageList | std::views::filter([&](const auto &image) {
                          return std::ranges::contains(image.repoTags, target);
                      }) |
                      std::ranges::to<std::vector>();
        log_info << "Images found, name: " << name << ", count: " << images.size();
        return images;
    }

    void ContainerService::DeleteImage(const std::string &id) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, id: " << id;
            return;
        }
        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::delete_, "/images/" + id + "?force=true"); statusCode != http::status::ok) {
            log_error << "Delete image failed, statusCode: " << statusCode << ", id: " << id;
            throw Core::ServiceException("Delete image failed, id: " + id, statusCode);
        }
        log_debug << "Image deleted, id: " << id;
    }

    bool ContainerService::ContainerExists(const std::string &containerName) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << containerName;
            return false;
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerName + "/json");
        if (statusCode == http::status::ok) {
            log_debug << "Docker container found, name: " << containerName;
            return true;
        }
        log_info << "Docker container not found, name: " << containerName << ", statusCode: " << statusCode;
        return false;
    }

    bool ContainerService::ContainerExistsByImageName(const std::string &imageName, const std::string &tag) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << imageName << ":" << tag;
            return false;
        }
        if (const std::vector<Dto::Docker::Container> containers = ListContainerByImageName(imageName, tag); containers.empty()) {
            log_info << "Docker container not found, name: " << (tag.empty() ? imageName : imageName + ":" + tag);
            return false;
        }
        return true;
    }

    Dto::Docker::Container ContainerService::GetFirstContainerByImageName(const std::string &name, const std::string &tag) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name << ":" << tag;
            return {};
        }
        const std::vector<Dto::Docker::Container> containers = ListContainerByImageName(name, tag);
        if (containers.empty()) {
            log_info << "Docker container not found, name: " << name << ":" << tag;
            return {};
        }
        if (containers.size() > 1) {
            log_info << "More than one docker container found, name: " << name << ":" << tag << " count: " << containers.size();
        }
        log_debug << "Docker container found, name: " << name << ":" << tag;
        return containers.front();
    }

    Dto::Docker::Container ContainerService::GetContainerById(const std::string &containerId) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, id: " << containerId;
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerId + "/json");
        if (statusCode != http::status::ok) {
            log_info << "Get docker container by ID failed, statusCode: " << statusCode;
            return {};
        }
        log_debug << "Docker container found, containerId: " << containerId;
        return Dto::Docker::Container::FromJson(body);
    }

    Dto::Docker::Container ContainerService::GetContainerByName(const std::string &name) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name;
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + name + "/json");
        if (statusCode != http::status::ok) {
            log_info << "Get container by name failed, name: " << name << ", statusCode: " << statusCode;
            return {};
        }
        log_debug << "Container found, name: " << name;
        return Dto::Docker::Container::FromJson(body);
    }

    Dto::Docker::InspectContainerResponse ContainerService::InspectContainer(const std::string &containerId) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, id: " << containerId;
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerId + "/json?size=true");
        if (statusCode != http::status::ok) {
            log_info << "Inspect container failed, containerId: " << Core::StringUtils::Continuation(containerId, 16) << ", statusCode: " << statusCode;
            Dto::Docker::InspectContainerResponse err{};
            err.status = statusCode;
            return err;
        }
        Dto::Docker::InspectContainerResponse response = Dto::Docker::InspectContainerResponse::FromJson(body);
        response.status = statusCode;
        log_debug << "Container inspected, containerId: " << containerId;
        return response;
    }

    Dto::Docker::ListContainerResponse ContainerService::ListContainers() const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/json?all=true");
        if (statusCode != http::status::ok) {
            log_info << "List docker containers failed, statusCode: " << statusCode;
            return {};
        }
        Dto::Docker::ListContainerResponse response = Dto::Docker::ListContainerResponse::FromJson(body);
        log_debug << "Docker containers found, count: " << response.containerList.size();
        return response;
    }

    std::vector<Dto::Docker::Container> ContainerService::ListContainerByImageName(const std::string &name, const std::string &tag) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name << ":" << tag;
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/json?all=true");
        if (statusCode != http::status::ok) {
            log_info << "List docker container by image name failed, name: " << name << ":" << tag << ", statusCode: " << statusCode;
            return {};
        }

        const Dto::Docker::ListContainerResponse response = Dto::Docker::ListContainerResponse::FromJson(body);
        const std::string target = tag.empty() ? name : name + ":" + tag;
        auto containers = response.containerList | std::views::filter([&](const auto &c) {
                              return c.image == target;
                          }) |
                          std::ranges::to<std::vector>();

        if (containers.empty()) {
            log_info << "Docker container not found, name: " << name << ":" << tag;
        } else {
            log_debug << "Docker container found, name: " << name << ":" << tag << ", count: " << containers.size();
        }
        return containers;
    }

    Dto::Docker::CreateContainerResponse ContainerService::SendCreateContainer(const std::string &imageName, const std::string &tag, const std::string &hostName, const std::string &urlName, const std::string &containerPortStr, const int hostPort,
                                                                               const std::vector<std::string> &environment) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << imageName << ":" << tag;
            return {};
        }

        Dto::Docker::CreateContainerRequest request;
        request.hostName = hostName;
        request.domainName = "awsmock";
        request.tty = false;
        request.image = imageName + ":" + tag;
        request.containerPort = containerPortStr;
        request.hostPort = std::to_string(hostPort);
        request.environment = environment;
        request.exposedPorts[containerPortStr + "/tcp"] = {};

        Dto::Docker::LogConfig logConfig;
        logConfig.type = "json-file";

        Dto::Docker::Port portBinding;
        portBinding.hostPort = hostPort;

        Dto::Docker::HostConfig hostConfig;
        hostConfig.networkMode = _networkName;
        hostConfig.logConfig = logConfig;
        hostConfig.portBindings[containerPortStr + "/tcp"] = {portBinding};
        hostConfig.extraHosts.emplace_back("host.docker.internal:host-gateway");
        hostConfig.extraHosts.emplace_back("awsmock:host-gateway");
        hostConfig.extraHosts.emplace_back("localstack:host-gateway");

        request.hostConfig = hostConfig;
        log_debug << "Create container request: " << request.ToJson();

        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/containers/create?name=" + urlName, request.ToJson());
        if (statusCode != http::status::created) {
            log_warning << "Create container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return {};
        }

        Dto::Docker::CreateContainerResponse response = Dto::Docker::CreateContainerResponse::FromJson(body);
        response.hostPort = hostPort;
        log_debug << "Docker container created, name: " << imageName << ":" << tag << " id: " << response.id;
        return response;
    }

    Dto::Docker::CreateContainerResponse ContainerService::CreateContainer(const std::string &imageName, const std::string &instanceName, const std::string &tag, const std::vector<std::string> &environment, const int hostPort) const {
        return SendCreateContainer(imageName, tag, instanceName, instanceName, _containerPort, hostPort, environment);
    }

    Dto::Docker::CreateContainerResponse ContainerService::CreateContainer(const std::string &imageName, const std::string &instanceName, const std::string &tag, const std::vector<std::string> &environment, const int hostPort,
                                                                           const int containerPort) const {
        return SendCreateContainer(imageName, tag, imageName, instanceName, std::to_string(containerPort), hostPort, environment);
    }

    Dto::Docker::CreateContainerResponse ContainerService::CreateContainer(const std::string &imageName, const std::string &tag, const std::string &containerName, const int hostPort, const int containerPort) const {
        return SendCreateContainer(imageName, tag, containerName, containerName, std::to_string(containerPort), hostPort, {});
    }

    Dto::Docker::ListStatsResponse ContainerService::ListContainerStats() const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return {};
        }

        Dto::Docker::ListStatsResponse response;
        const Dto::Docker::ListContainerResponse listResponse = ListContainers();
        for (const auto &container: listResponse.containerList) {
            auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + container.id + "/stats?stream=false&one-shot=true");
            if (statusCode != http::status::ok) {
                log_warning << "List container stats failed, statusCode: " << statusCode << " body " << body;
                return {};
            }
            Dto::Docker::ContainerStat containerStat = Dto::Docker::ContainerStat::FromJson(body);
            containerStat.containerId = container.id;
            containerStat.state = container.state;
            containerStat.name = container.GetContainerName();
            response.containerStats.emplace_back(containerStat);
        }
        response.total = static_cast<long>(listResponse.containerList.size());
        return response;
    }

    void ContainerService::ContainerAttach(const std::string &containerId, boost::beast::websocket::stream<boost::beast::tcp_stream> &ws, long tail) const {
        if (containerId.empty()) {
            log_error << "Empty container Id";
            return;
        }
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return;
        }
        ws.control_callback([&ws](const boost::beast::websocket::frame_type kind, const boost::beast::string_view message) {
            if (kind == boost::beast::websocket::frame_type::close) {
                ws.close(boost::beast::websocket::close_code::abnormal);
            } else if (kind == boost::beast::websocket::frame_type::ping) {
                ws.pong(boost::beast::websocket::ping_data(message));
            }
        });

        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerId + "/logs?tail=" + std::to_string(tail) + "&stdout=true&stderr=true"); statusCode == http::status::ok && contentLength > 0) {
            ws.text(true);
            ws.write(boost::asio::buffer(Core::StringUtils::RemoveColorCoding(body)));
        }

        try {
            boost::asio::streambuf buffer;
            system_clock::time_point last = system_clock::now();
            while (ws.is_open()) {
                const std::string since = std::to_string(Core::DateTimeUtils::UnixTimestamp(last));
                if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerId + "/logs?since=" + since + "&stdout=true&stderr=true&tail=1000"); statusCode == http::status::ok && contentLength > 0) {
                    if (ws.is_open()) {
                        ws.text(true);
                        ws.write(boost::asio::buffer(Core::StringUtils::RemoveColorCoding(body)));
                    }
                }
                last = system_clock::now();

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
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available, name: " << name;
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/networks/" + name);
        if (statusCode == http::status::ok) {
            log_debug << "Docker network found, name: " << name;
            return true;
        }
        log_debug << "Docker network not found, name: " << name << ", statusCode: " << statusCode;
        return false;
    }

    Dto::Docker::CreateNetworkResponse ContainerService::CreateNetwork(const Dto::Docker::CreateNetworkRequest &request) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/networks/create", request.ToJson());
        if (statusCode != http::status::ok) {
            log_error << "Docker network create failed, name: " << request.name << ", statusCode: " << statusCode;
            return {};
        }
        log_debug << "Docker network created, name: " << request.name << " driver: " << request.driver;
        return Dto::Docker::CreateNetworkResponse::FromJson(body);
    }

    void ContainerService::StartContainer(const std::string &containerId, const std::string &containerName) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return;
        }
        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/containers/" + containerId + "/start"); statusCode != http::status::ok && statusCode != http::status::no_content) {
            log_warning << "Start container failed, name: " << containerName << ", id: " << containerId.substr(0, 12) << ", statusCode: " << statusCode;
            return;
        }
        log_debug << "Docker container started, id: " << containerId;
    }

    bool ContainerService::IsContainerRunning(const std::string &containerId) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerId + "/json");
        if (statusCode != http::status::ok) {
            log_debug << "Is docker container running failed, id: " << containerId;
            return false;
        }
        const Dto::Docker::InspectContainerResponse response = Dto::Docker::InspectContainerResponse::FromJson(body);
        log_debug << "Docker container state, id: " << containerId << " running: " << std::boolalpha << response.state.running;
        return response.state.running;
    }

    void ContainerService::WaitForContainer(const std::string &containerId) const {
        const int checkTime = Core::Configuration::instance().get<int>("awsmock.docker.container.checkTime");
        const int maxWaitTime = Core::Configuration::instance().get<int>("awsmock.docker.container.maxWaitTime");
        const auto deadline = system_clock::now() + std::chrono::seconds{maxWaitTime};
        while (!IsContainerRunning(containerId) && system_clock::now() < deadline) {
            std::this_thread::sleep_for(std::chrono::milliseconds(checkTime));
        }
    }

    void ContainerService::RestartContainer(const Dto::Docker::Container &container) const {
        RestartDockerContainer(container.id);
    }

    void ContainerService::RestartDockerContainer(const std::string &containerId) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return;
        }
        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/containers/" + containerId + "/restart"); statusCode != http::status::no_content) {
            log_warning << "Restart container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container restarted, containerId: " << containerId;
    }

    std::string ContainerService::GetContainerLogs(const std::string &containerId, const system_clock::time_point &start) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return {};
        }
        const std::string since = std::to_string(Core::DateTimeUtils::UnixTimestamp(start));
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerId + "/logs?since=" + since + "&stdout=true&stderr=true");
        if (statusCode != http::status::ok) {
            log_error << "Receive container logs failed, containerId: " << containerId << ", statusCode: " << statusCode;
            return {};
        }
        log_debug << "Container logs received, containerId: " << containerId;
        return body;
    }

    Dto::Docker::ContainerStat ContainerService::GetContainerStats(const std::string &containerId, const std::string &applicationName) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return {};
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::get, "/containers/" + containerId + "/stats?stream=false");
        if (statusCode != http::status::ok) {
            log_error << "Get container stats failed, name: " << applicationName << ", containerId: " << containerId << ", statusCode: " << statusCode;
            return {};
        }
        log_debug << "Container statistics received, containerId: " << containerId;
        return Dto::Docker::ContainerStat::FromJson(body);
    }

    void ContainerService::StopContainer(const Dto::Docker::Container &container) const {
        StopContainer(container.id);
    }

    void ContainerService::StopContainer(const std::string &containerId) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return;
        }
        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/containers/" + containerId + "/stop"); statusCode != http::status::no_content && statusCode != http::status::not_modified) {
            log_warning << "Stop container failed, statusCode: " << statusCode;
            return;
        }
        log_debug << "Docker container stopped, id: " << containerId;
    }

    void ContainerService::KillContainer(const Dto::Docker::Container &container, const std::string &signal) const {
        KillContainer(container.id, signal);
    }

    void ContainerService::KillContainer(const std::string &containerId, const std::string &signal) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return;
        }
        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/containers/" + containerId + "/kill?signal=" + signal); statusCode != http::status::no_content) {
            log_warning << "Kill container failed, containerId: " << containerId << ", statusCode: " << statusCode;
            return;
        }
        log_debug << "Docker container killed, id: " << containerId << ", signal: " << signal;
    }

    void ContainerService::DeleteContainer(const Dto::Docker::Container &container) const {
        DeleteContainer(container.id);
    }

    void ContainerService::DeleteContainer(const std::string &containerId) const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return;
        }
        if (auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::delete_, "/containers/" + containerId + "?force=true"); statusCode != http::status::no_content) {
            log_warning << "Delete container failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        log_debug << "Docker container deleted, id: " << containerId;
    }

    void ContainerService::DeleteContainers(const std::string &imageName, const std::string &tag) const {
        for (const auto &container: ListContainerByImageName(imageName, tag)) {
            DeleteContainer(container.id);
        }
        log_debug << "All docker containers deleted, image: " << imageName << ":" << tag;
    }

    void ContainerService::PruneContainers() const {
        if (!_initialized) {
            log_warning << "Docker not initialized, container commands not available";
            return;
        }
        auto [statusCode, body, contentLength] = GetSocket()->SendJson(http::verb::post, "/containers/prune");
        if (statusCode != http::status::ok) {
            log_warning << "Prune containers failed, statusCode: " << statusCode << ", body: " << Core::StringUtils::StripLineEndings(body);
            return;
        }
        const Dto::Docker::PruneContainerResponse response = Dto::Docker::PruneContainerResponse::FromJson(body);
        log_debug << "Prune containers, count: " << response.containersDeleted.size() << " spaceReclaimed: " << response.spaceReclaimed;
    }

    std::string ContainerService::WriteLambdaDockerFile(const std::string &codeDir, const std::string &handler, const std::string &runtime, const std::map<std::string, std::string> &environment) const {
        const std::string dockerFilename = Core::FileUtils::appendPath(codeDir, "Dockerfile");
        std::string providedRuntime = boost::algorithm::to_lower_copy(runtime);
        Core::StringUtils::Replace(providedRuntime, ".", "-");
        const auto supportedRuntime = Core::Configuration::instance().get<std::string>("awsmock.modules.lambda.runtime." + providedRuntime);
        const auto region = Core::Configuration::instance().get<std::string>("awsmock.region");
        log_debug << "Using supported runtime: " << supportedRuntime;

        const std::string credentialsCopy = AddCredentials(codeDir, region);
        const std::string envLines = AddEnvironment(environment);

        std::ofstream ofs(dockerFilename);
        ofs << "FROM " << supportedRuntime << "\n";
        ofs << envLines;

        if (Core::StringUtils::StartsWithIgnoringCase(runtime, "java")) {
            ofs << "RUN mkdir -p /root/.aws\n";
            ofs << credentialsCopy;
            ofs << "COPY classes ${LAMBDA_TASK_ROOT}\n";
            ofs << "CMD [ \"" + handler + "::handleRequest\" ]\n";
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "postgres")) {
            ofs << "CMD [ \"" + handler + "\" ]\n";
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "provided")) {
            ofs << credentialsCopy;
            ofs << "COPY bootstrap ${LAMBDA_RUNTIME_DIR}\n";
            ofs << "RUN chmod 775 ${LAMBDA_RUNTIME_DIR}/bootstrap\n";
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/lib\n";
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/bin\n";
            ofs << "COPY bin/* ${LAMBDA_TASK_ROOT}/bin/\n";
            ofs << "COPY lib/* ${LAMBDA_TASK_ROOT}/lib/\n";
            ofs << "RUN chmod 775 -R ${LAMBDA_TASK_ROOT}/lib\n";
            ofs << "RUN chmod 775 -R ${LAMBDA_TASK_ROOT}/bin\n";
            ofs << "CMD [ \"" + handler + "\" ]\n";
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "python")) {
            ofs << "COPY requirements.txt ${LAMBDA_TASK_ROOT}/\n";
            ofs << "RUN pip install -r ${LAMBDA_TASK_ROOT}/requirements.txt\n";
            ofs << "RUN mkdir -p /root/.aws\n";
            ofs << credentialsCopy;
            ofs << "COPY *.py ${LAMBDA_TASK_ROOT}/\n";
            ofs << "CMD [\"" + handler + "\"]\n";
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "nodejs22")) {
            ofs << "RUN mkdir -p ${LAMBDA_TASK_ROOT}/dist\n";
            if (Core::DirUtils::DirectoryExists(Core::FileUtils::appendPath(codeDir, "node_modules"))) {
                ofs << "COPY node_modules/ ${LAMBDA_TASK_ROOT}/node_modules/\n";
            }
            ofs << "COPY " << GetHandlerFileNodeJs22(handler) << " ${LAMBDA_TASK_ROOT}/dist\n";
            ofs << "CMD [\"" + handler + "\"]\n";
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "nodejs")) {
            if (Core::DirUtils::DirectoryExists(Core::FileUtils::appendPath(codeDir, "node_modules"))) {
                ofs << "COPY node_modules/ ${LAMBDA_TASK_ROOT}/node_modules/\n";
            }
            ofs << "COPY index.js ${LAMBDA_TASK_ROOT}\n";
            ofs << "CMD [\"" + handler + "\"]\n";
        } else if (Core::StringUtils::StartsWithIgnoringCase(runtime, "go")) {
            ofs << "COPY bootstrap ${LAMBDA_RUNTIME_DIR}\n";
            ofs << "RUN chmod 755 ${LAMBDA_RUNTIME_DIR}/bootstrap\n";
            ofs << "CMD [\"" + handler + "\"]\n";
        }

        log_debug << "Dockerfile written, filename: " << dockerFilename;
        return dockerFilename;
    }

    std::string ContainerService::WriteApplicationDockerFile(const std::string &codeDir, const Database::Entity::Apps::Application &applicationEntity) const {
        const std::string dockerFilename = Core::FileUtils::appendPath(codeDir, "Dockerfile");
        std::string dockerFileContent = applicationEntity.dockerFile;
        Core::StringUtils::Replace(dockerFileContent, "$$ENV$$", AddEnvironment(applicationEntity.environment));
        Core::StringUtils::Replace(dockerFileContent, "$$PORT$$", std::to_string(applicationEntity.privatePort));
        Core::StringUtils::Replace(dockerFileContent, "$$ARCHIVE$$", applicationEntity.archive);
        Core::StringUtils::Replace(dockerFileContent, "$$CREDENTIALS$$", AddCredentials(codeDir, applicationEntity.region));

        std::ofstream ofs(dockerFilename);
        ofs << dockerFileContent << "\n";
        log_debug << "Dockerfile written, filename: " << dockerFilename;
        return dockerFilename;
    }

    std::string ContainerService::BuildImageFile(const std::string &codeDir, const std::string &name) const {
#ifdef _WIN32
        const std::string tarFileName = Core::FileUtils::appendPath(codeDir, name + ".tar");
#else
        const std::string tarFileName = Core::FileUtils::appendPath(codeDir, name + ".tgz");
#endif
        Core::TarUtils::TarDirectory(tarFileName, codeDir);
        log_debug << "Zipped TAR file written: " << tarFileName;
        return tarFileName;
    }

    std::string ContainerService::GetNetworkName() {
        return Core::Configuration::instance().get<std::string>("awsmock.docker.network-name");
    }

    std::string ContainerService::GetHandlerFileNodeJs22(const std::string &handler) {
        return handler.substr(0, handler.find('.')) + ".mjs";
    }

    std::string ContainerService::AddEnvironment(const std::map<std::string, std::string> &environment) {
        std::stringstream ss;
        for (const auto &[fst, snd]: environment) {
            ss << "ENV " << fst << "=\"" << snd << "\"\n";
        }
        ss << "ENV AWS_REGION=\"eu-central-1\"\n";
        ss << "ENV AWS_ACCESS_KEY_ID=\"none\"\n";
        ss << "ENV AWS_SECRET_ACCESS_KEY=\"none\"\n";
        ss << "ENV AWS_SESSION_TOKEN=\"none\"\n";
        return ss.str();
    }

    std::string ContainerService::AddCredentials(const std::string &codeDir, const std::string &region) {

        std::ofstream credOfs(Core::FileUtils::appendPath(codeDir, "credentials"));
        credOfs << "[default]\n"
                << "region=" << region << "\n"
                << "aws_access_key_id=none\n"
                << "aws_secret_access_key=none\n"
                << "aws_session_token=none\n"
                << "retry_mode=standard\n"
                << "max_attempts=1\n";

        std::ofstream cfgOfs(Core::FileUtils::appendPath(codeDir, "config"));
        cfgOfs << "[default]\n"
                << "region=" << region << "\n"
                << "output=json\n";

        return "COPY credentials /root/.aws/\nCOPY config /root/.aws/\n";
    }

} // namespace Awsmock::Service
