
//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/apps/ApplicationService.h>

namespace AwsMock::Service {

    ApplicationService::ApplicationService(boost::asio::io_context &ioc) : _database(Database::ApplicationDatabase::instance()), _ioc(ioc) {
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::CreateApplication(const Dto::Apps::CreateApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "create_application");
        log_debug << "Create application request, region:  " << request.region << " name: " << request.application.name;

        if (_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application exists already, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application exists already, region: " + request.region + " name: " + request.application.name);
        }

        try {
            // Generate application ID
            Database::Entity::Apps::Application application = Dto::Apps::Mapper::map(request.application);
            application.region = request.region;
            application.created = system_clock::now();
            application.modified = system_clock::now();
            application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
            application = _database.CreateApplication(application);

            // Save the base64 encoded file
            const std::string fullBase64File = WriteBase64File(request.code, application, request.application.version);

            // Create the application asynchronously
            const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
            ApplicationCreator applicationCreator;
            boost::asio::post(_ioc, [applicationCreator, fullBase64File, application, instanceId] {
                applicationCreator(fullBase64File, application.region, application.name, instanceId);
            });

            Dto::Apps::ListApplicationCountersRequest listRequest{};
            listRequest.requestId = request.requestId;
            listRequest.region = request.region;
            listRequest.user = request.user;
            listRequest.prefix = request.prefix;
            listRequest.pageSize = request.pageSize;
            listRequest.pageIndex = request.pageIndex;
            log_trace << "Application created, application: " + application.ToJson();
            return ListApplications(listRequest);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Apps::GetApplicationResponse ApplicationService::GetApplication(const Dto::Apps::GetApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "get_application");
        log_debug << "Get application request, region:  " << request.region << " name: " << request.name;

        if (!_database.ApplicationExists(request.region, request.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.name);
        }

        try {
            const Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.name);

            Dto::Apps::GetApplicationResponse getRequest{};
            getRequest.requestId = request.requestId;
            getRequest.region = request.region;
            getRequest.user = request.user;
            getRequest.application = Dto::Apps::Mapper::map(application);
            log_trace << "Application retrieved, application: " + application.ToJson();
            return getRequest;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Apps::GetApplicationResponse ApplicationService::UpdateApplication(const Dto::Apps::UpdateApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "update_application");
        log_debug << "Get application request, region:  " << request.region << " name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.application.name);
        }

        try {
            Database::Entity::Apps::Application application = Dto::Apps::Mapper::map(request.application);
            application = _database.UpdateApplication(application);

            // Stop if not enabled anymore
            if (!application.enabled) {
                Dto::Apps::StopApplicationRequest stopRequest{};
                stopRequest.requestId = request.requestId;
                stopRequest.region = request.region;
                stopRequest.user = request.user;
                stopRequest.application = Dto::Apps::Mapper::map(application);
                StopApplication(stopRequest);
            } else {
                Dto::Apps::StartApplicationRequest startRequest{};
                startRequest.requestId = request.requestId;
                startRequest.region = request.region;
                startRequest.user = request.user;
                startRequest.application = Dto::Apps::Mapper::map(application);
                StartApplication(startRequest);
            }

            // Create get request
            Dto::Apps::GetApplicationResponse getRequest{};
            getRequest.requestId = request.requestId;
            getRequest.region = request.region;
            getRequest.user = request.user;
            getRequest.application = Dto::Apps::Mapper::map(application);
            log_trace << "Application updated, application: " + application.ToJson();
            return getRequest;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void ApplicationService::UploadApplicationCode(const Dto::Apps::UploadApplicationCodeRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "upload_application_code");
        log_debug << "Upload application code request, name: " << request.applicationName;

        // Check existence
        if (!_database.ApplicationExists(request.region, request.applicationName)) {
            log_warning << "Application does not exist, name: " << request.applicationName;
            throw Core::ServiceException("Application does not exist, name: " + request.applicationName);
        }

        // Check code length
        if (request.applicationCode.empty()) {
            log_warning << "Application code is empty, name: " << request.applicationName;
            throw Core::ServiceException("Lambda function code is empty, name: " + request.applicationName);
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.applicationName);

        // Save the base64 encoded file
        const std::string fullBase64File = WriteBase64File(request.applicationCode, application, request.version);
        log_debug << "Base64 application written, base64File: " << fullBase64File;

        // Set status and version
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application.version = request.version;
        application.archive = request.archive;
        application = _database.UpdateApplication(application);

        // Delete container and image
        CleanupDocker(application);

        // Create the application asynchronously
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        ApplicationCreator applicationCreator;
        boost::asio::post(_ioc, [applicationCreator, fullBase64File, application, instanceId] {
            applicationCreator(fullBase64File, application.region, application.name, instanceId);
        });
        log_debug << "Application code updated, name: " << request.applicationName << ", version: " << request.version;
    }

    void ApplicationService::RebuildApplication(const Dto::Apps::RebuildApplicationCodeRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "rebuild_application");
        log_debug << "Rebuild application code request, name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_warning << "Application does not exist, name: " << request.application.name;
            throw Core::ServiceException("Application does not exist, name: " + request.application.name);
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);

        // Set status and version
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application = _database.UpdateApplication(application);

        // Delete container and image
        CleanupDocker(application);

        // Get the base64 encoded application code
        const auto applicationDataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.data-dir");
        const std::string base64File = application.name + "-" + application.version + ".b64";
        std::string base64FullFile = applicationDataDir + Core::FileUtils::separator() + base64File;
        log_debug << "Using Base64File: " << base64FullFile;

        // Create the application asynchronously
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        ApplicationCreator applicationCreator;
        boost::asio::post(_ioc, [applicationCreator, base64FullFile, application, instanceId] {
            applicationCreator(base64FullFile, application.region, application.name, instanceId);
        });
    }

    void ApplicationService::EnableApplication(const Dto::Apps::EnableApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "enable_application");
        log_debug << "Enable application request, name: " << request.application.name;

        // Check existence
        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_warning << "Application does not exist, name: " << request.application.name;
            return;
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);
        application.enabled = true;
        application = _database.UpdateApplication(application);
        log_debug << "Application enabled, name: " << request.application.name;
    }

    void ApplicationService::EnableAllApplications(const Dto::Apps::EnableAllApplicationsRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "enable_all_applications");
        log_debug << "Enable all applications request, region: " << request.region;

        // Get application
        for (std::vector<Database::Entity::Apps::Application> applications = _database.ListApplications(request.region); auto &application: applications) {
            application.enabled = true;
            application = _database.UpdateApplication(application);
        }
        log_debug << "All applications enabled, region: " << request.region;
    }

    void ApplicationService::DisableApplication(const Dto::Apps::DisableApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, "action", "disable_application");
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_SERVICE_COUNTER, "action", "disable_application");
        log_debug << "Diable application request, name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_warning << "Application does not exist, name: " << request.application.name;
            return;
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);
        application.enabled = false;
        application = _database.UpdateApplication(application);
        log_debug << "Application disabled, name: " << request.application.name;
    }

    void ApplicationService::DisableAllApplications(const Dto::Apps::DisableAllApplicationsRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "disable_all_applications");
        log_debug << "Disable all applications request, region: " << request.region;

        // Get application
        for (std::vector<Database::Entity::Apps::Application> applications = _database.ListApplications(request.region); auto &application: applications) {
            application.enabled = false;
            application = _database.UpdateApplication(application);
        }
        log_debug << "All applications disabled, region: " << request.region;
    }

    void ApplicationService::StartApplication(const Dto::Apps::StartApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "start_application");
        log_debug << "Start application request, name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_warning << "Application does not exist, name: " << request.application.name;
            return;
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);

        // Check enables flag
        if (!application.enabled) {
            log_warning << "Application not enabled, name: " << request.application.name;
            return;
        }

        // Update status
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application.enabled = true;
        application = _database.UpdateApplication(application);

        // Get code base64 file name
        auto dataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.data-dir");
        std::string fullBase64File = Core::FileUtils::appendPath(dataDir, "/", application.name, "-", application.version, ".b64");

        // Get container id, if already running
        if (application.containerName.empty()) {
            if (const Dto::Docker::Container container = ContainerService::instance().GetFirstContainerByImageName(application.name, application.version); !container.id.empty()) {
                application.imageName = container.image;
                application.containerId = container.id;
                application.containerName = container.GetContainerName();
                application = _database.UpdateApplication(application);
            }
        }

        // Check whether a container exists already
        if (!ContainerService::instance().ContainerExistsByImageName(application.name, application.version)) {

            // Create the application asynchronously
            const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
            ApplicationCreator applicationCreator;
            boost::asio::post(_ioc, [applicationCreator, fullBase64File, application, instanceId] {
                applicationCreator(fullBase64File, application.region, application.name, instanceId);
            });
            log_debug << "Application start initiated, name: " << request.application.name;

        } else {
            Dto::Docker::InspectContainerResponse inspectContainerResponse = ContainerService::instance().InspectContainer(application.containerId);
            if (inspectContainerResponse.status == http::status::ok && inspectContainerResponse.state.status != "running") {
                ContainerService::instance().StartDockerContainer(inspectContainerResponse.id, inspectContainerResponse.name);
                ContainerService::instance().WaitForContainer(inspectContainerResponse.id);
            }

            const Dto::Docker::Container container = ContainerService::instance().GetFirstContainerByImageName(application.name, application.version);
            inspectContainerResponse = ContainerService::instance().InspectContainer(container.id);
            if (inspectContainerResponse.status == http::status::ok) {
                application.imageId = inspectContainerResponse.image;
                application.containerId = inspectContainerResponse.id;
                application.containerName = inspectContainerResponse.name.substr(1);
                application.publicPort = inspectContainerResponse.hostConfig.GetFirstPublicPort(std::to_string(application.privatePort));
                application.status = inspectContainerResponse.state.status == "running" ? Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::RUNNING) : Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                application = _database.UpdateApplication(application);
                log_info << "Application started, name: " << request.application.name << ", publicPort: " << application.publicPort;
            } else {
                log_error << "Could not get the status of the container, name: " << application.containerName;
            }
        }
    }

    long ApplicationService::StartAllApplications() const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "start_all_applications");
        log_debug << "Start all applications request";

        // Loop over applications
        long count = 0;
        for (auto &application: _database.ListApplications()) {
            if (application.enabled) {
                Dto::Apps::StartApplicationRequest startRequest;
                startRequest.application = Dto::Apps::Mapper::map(application);
                startRequest.region = application.region;
                StartApplication(startRequest);
                count++;
            }
        }
        return count;
    }

    void ApplicationService::RestartApplication(const Dto::Apps::RestartApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "restart_application");
        log_debug << "Restart application request, name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_warning << "Application does not exist, name: " << request.application.name;
            return;
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);

        // Update status
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application = _database.UpdateApplication(application);

        // Stop the application
        if (!ContainerService::instance().ContainerExists(application.containerName)) {
            log_warning << "Container does not exist, name: " << request.application.name;
            return;
        }

        // Stop container
        ContainerService::instance().KillContainer(application.containerId);
        ContainerService::instance().DeleteContainer(application.containerId);
        log_debug << "Application stopped, name: " << application.name;

        const auto dataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.data-dir");
        const auto fullBase64File = dataDir + Core::FileUtils::separator() + Core::FileUtils::separator() + application.name + "-" + application.version + ".b64";

        // Check whether a container exists already
        if (application.containerName.empty() || !ContainerService::instance().ContainerExistsByImageName(application.name, application.version)) {

            // Create the application asynchronously
            const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
            ApplicationCreator applicationCreator;
            boost::thread t(boost::ref(applicationCreator), fullBase64File, application.region, application.name, instanceId);
            t.detach();
            log_debug << "Application start initiated, name: " << request.application.name;

        } else {

            Dto::Docker::InspectContainerResponse inspectContainerResponse = ContainerService::instance().InspectContainer(application.containerId);
            if (inspectContainerResponse.status == http::status::ok && inspectContainerResponse.state.status != "running") {
                ContainerService::instance().StartDockerContainer(inspectContainerResponse.id, inspectContainerResponse.name);
                ContainerService::instance().WaitForContainer(inspectContainerResponse.id);
            }

            const Dto::Docker::Container container = ContainerService::instance().GetFirstContainerByImageName(application.name, application.version);
            inspectContainerResponse = ContainerService::instance().InspectContainer(container.id);
            if (inspectContainerResponse.status == http::status::ok) {
                application.imageId = inspectContainerResponse.image;
                application.containerId = inspectContainerResponse.id;
                application.containerName = inspectContainerResponse.name.substr(1);
                application.publicPort = inspectContainerResponse.hostConfig.GetFirstPublicPort(std::to_string(application.privatePort));
                application.status = inspectContainerResponse.state.status == "running" ? Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::RUNNING) : Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                application = _database.UpdateApplication(application);
                log_info << "Application already started, name: " << request.application.name << ", publicPort: " << application.publicPort;
            } else {
                log_error << "Could not get the status of the container, name: " << application.containerName;
            }
        }
    }

    long ApplicationService::RestartAllApplications() const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "restart_all_applications");
        log_debug << "Restart all applications request";

        long count = StopAllApplications();
        count = StartAllApplications();
        return count;
    }

    void ApplicationService::StopApplication(const Dto::Apps::StopApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "stop_application");
        log_debug << "Stop application request, region:  " << request.region << " name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.application.name);
        }

        try {
            Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);

            if (application.containerName.empty() || !ContainerService::instance().ContainerExists(application.containerName)) {
                log_warning << "Container does not exist, name: " << request.application.name;
                return;
            }

            // Stop container
            ContainerService::instance().KillContainer(application.containerName);
            application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
            application.containerId = "";
            application.containerName = "";
            application.enabled = false;
            application = _database.UpdateApplication(application);
            log_debug << "Application stopped, name: " << application.name;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    long ApplicationService::StopAllApplications() const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "stop_all_applications");
        log_debug << "Stop application request";

        try {
            long count = 0;
            for (auto &application: _database.ListApplications()) {
                Dto::Apps::StopApplicationRequest stopRequest;
                stopRequest.application = Dto::Apps::Mapper::map(application);
                stopRequest.region = application.region;
                StopApplication(stopRequest);
                count++;
            }
            return count;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::ListApplications(const Dto::Apps::ListApplicationCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "list_applications");
        log_debug << "List applications request";

        try {

            Dto::Apps::ListApplicationCountersResponse response;

            const std::vector<Database::Entity::Apps::Application> applications = _database.ListApplications(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            response.total = _database.CountApplications(request.region, request.prefix);
            log_trace << "Got applications, region: " << request.region;

            // Prepare response
            response.region = request.region;
            response.user = request.user;
            response.requestId = request.requestId;
            response.applications = Dto::Apps::Mapper::map(applications);
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::vector<std::string> ApplicationService::ListApplicationNames() const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "list_application_names");
        log_debug << "List application names request";

        try {

            const std::vector<Database::Entity::Apps::Application> applications = _database.ListApplications();

            // Prepare response
            std::vector<std::string> response;
            for (const auto &application: applications) {
                response.push_back(application.name);
            }
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::DeleteApplication(const Dto::Apps::DeleteApplicationRequest &request) const {
        Monitoring::MonitoringTimer measure(APPLICATION_SERVICE_TIMER, APPLICATION_SERVICE_COUNTER, "action", "delete_application");
        log_debug << "Delete application request, region:  " << request.region << " name: " << request.name;

        if (!_database.ApplicationExists(request.region, request.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.name);
        }

        try {

            const long count = _database.DeleteApplication(request.region, request.name);
            log_debug << "Application deleted, count: " << count;

            Dto::Apps::ListApplicationCountersRequest listRequest{};
            listRequest.requestId = request.requestId;
            listRequest.region = request.region;
            listRequest.user = request.user;
            listRequest.prefix = request.prefix;
            listRequest.pageSize = request.pageSize;
            listRequest.pageIndex = request.pageIndex;
            log_trace << "Application deleted, name: " + request.name;
            return ListApplications(listRequest);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ApplicationService::WriteBase64File(const std::string &applicationCode, Database::Entity::Apps::Application &application, const std::string &version) {

        auto applicationDataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.data-dir");

        std::string base64File = application.name + "-" + version + ".b64";
        std::string base64FullFile = applicationDataDir + Core::FileUtils::separator() + base64File;
        log_debug << "Using Base64File: " << base64FullFile;

        // If we do not have a local file already or the MD5 sum changed, write the Base64 encoded file to lambda dir
        if (!Core::FileUtils::FileExists(base64FullFile)) {

            std::ofstream ofs(base64FullFile);
            ofs << applicationCode;
            ofs.close();
            log_debug << "New Base64 application file written: " << base64FullFile;

        } else {

            std::string md5sumFile = Core::Crypto::GetMd5FromFile(base64FullFile);
            if (std::string md5sumString = Core::Crypto::GetMd5FromString(applicationCode); md5sumFile != md5sumString) {

                std::ofstream ofs(base64FullFile);
                ofs << applicationCode;
                ofs.close();
                log_debug << "Updated Base64 file written: " << base64FullFile;

            } else {
                log_info << "Application upload: new and original are equal: " << base64FullFile;
            }
        }
        return base64FullFile;
    }

    void ApplicationService::CleanupDocker(const Database::Entity::Apps::Application &application) {

        if (ContainerService::instance().ContainerExists(application.containerId)) {
            ContainerService::instance().KillContainer(application.containerId);
            ContainerService::instance().DeleteContainer(application.containerId);
            log_debug << "Done cleanup application container, name: " << application.name << ":" << application.version << ", containerId: " << application.containerId;
        }

        // Delete image
        if (ContainerService::instance().ImageExists(application.name, application.version)) {
            ContainerService::instance().DeleteImage(application.name + ":" + application.version);
            log_debug << "Done cleanup application image, name: " << application.name << ":" << application.version << ", containerId: " << application.containerId;
        }
        log_info << "Done cleanup docker, name: " << application.name << ":" << application.version << ", containerId: " << application.containerId;
    }

}// namespace AwsMock::Service
