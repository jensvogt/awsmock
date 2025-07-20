
//
// Created by vogje01 on 30/05/2023.
//

#include "awsmock/service/apps/ApplicationCreator.h"


#include <awsmock/service/apps/ApplicationService.h>

namespace AwsMock::Service {

    ApplicationService::ApplicationService() : _database(Database::ApplicationDatabase::instance()) {
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::CreateApplication(const Dto::Apps::CreateApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "create_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "create_application");
        log_debug << "Create application request, region:  " << request.region << " name: " << request.application.name;

        if (_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application exists already, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application exists already, region: " + request.region + " name: " + request.application.name);
        }

        try {
            // Generate application ID
            Database::Entity::Apps::Application application = Dto::Apps::Mapper::map(request.application);
            application.region = request.region;
            application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
            application = _database.CreateApplication(application);

            // Save the base64 encoded file
            const std::string fullBase64File = WriteBase64File(request.code, application, request.application.version);

            // Create the application asynchronously
            const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
            ApplicationCreator applicationCreator;
            boost::thread t(boost::ref(applicationCreator), fullBase64File, application.region, application.name, instanceId);
            t.detach();

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
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "get_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "get_application");
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
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "update_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "update_application");
        log_debug << "Get application request, region:  " << request.region << " name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.application.name);
        }

        try {
            Database::Entity::Apps::Application application = Dto::Apps::Mapper::map(request.application);
            application = _database.UpdateApplication(application);

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
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "action", "upload_application_code");
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_SERVICE_COUNTER, "action", "upload_application_code");
        log_debug << "Upload application code request, name: " << request.applicationName;

        if (!_database.ApplicationExists(request.region, request.applicationName)) {
            log_warning << "Application does not exist, name: " << request.applicationName;
            throw Core::ServiceException("Application does not exist, name: " + request.applicationName);
        }

        if (request.applicationCode.empty()) {
            log_warning << "Application code is empty, name: " << request.applicationName;
            throw Core::ServiceException("Lambda function code is empty, name: " + request.applicationName);
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.applicationName);

        // Save the base64 encoded file
        const std::string fullBase64File = WriteBase64File(request.applicationCode, application, request.version);

        // Set status and version
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application.version = request.version;
        application = _database.UpdateApplication(application);

        // Delete container and image
        DeleteImage(application);

        // Create the application asynchronously
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        ApplicationCreator applicationCreator;
        boost::thread t(boost::ref(applicationCreator), fullBase64File, application.region, application.name, instanceId);
        t.detach();

        log_debug << "Application code updated, name: " << request.applicationName;
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::RebuildApplication(const Dto::Apps::RebuildApplicationCodeRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "action", "rebuild_application");
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_SERVICE_COUNTER, "action", "rebuild_application");
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
        DeleteImage(application);

        // Get the base64 encoded application code
        const auto applicationDataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.application.data-dir");
        const std::string base64File = application.name + "-" + application.version + ".b64";
        std::string base64FullFile = applicationDataDir + Core::FileUtils::separator() + base64File;
        log_debug << "Using Base64File: " << base64FullFile;

        // Create the application asynchronously
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);
        ApplicationCreator applicationCreator;
        boost::thread t(boost::ref(applicationCreator), base64FullFile, application.region, application.name, instanceId);
        t.detach();

        Dto::Apps::ListApplicationCountersRequest listRequest{};
        listRequest.requestId = request.requestId;
        listRequest.region = request.region;
        listRequest.user = request.user;
        listRequest.prefix = request.prefix;
        listRequest.pageSize = request.pageSize;
        listRequest.pageIndex = request.pageIndex;
        log_debug << "Application rebuild, name: " << request.application.name;
        return ListApplications(listRequest);
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::StartApplication(const Dto::Apps::StartApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "action", "start_application");
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_SERVICE_COUNTER, "action", "start_application");
        log_debug << "Start application request, name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_warning << "Application does not exist, name: " << request.application.name;
            throw Core::ServiceException("Application does not exist, name: " + request.application.name);
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);

        // Update status
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application = _database.UpdateApplication(application);

        // Get code base64 file name
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

            Dto::Docker::Container container = ContainerService::instance().GetFirstContainerByImageName(application.name, application.version);
            inspectContainerResponse = ContainerService::instance().InspectContainer(container.id);
            if (inspectContainerResponse.status == http::status::ok) {
                application.imageId = inspectContainerResponse.image;
                application.containerId = inspectContainerResponse.id;
                application.containerName = inspectContainerResponse.name.substr(1);
                application.publicPort = inspectContainerResponse.hostConfig.portBindings.GetFirstPublicPort(std::to_string(application.privatePort));
                application.status = inspectContainerResponse.state.status == "running" ? Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::RUNNING) : Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                application = _database.UpdateApplication(application);
                log_info << "Application already started, name: " << request.application.name << ", publicPort: " << application.publicPort;
            } else {
                log_error << "Could not get the status of the container, name: " << application.containerName;
            }
        }

        Dto::Apps::ListApplicationCountersRequest listRequest{};
        listRequest.requestId = request.requestId;
        listRequest.region = request.region;
        listRequest.user = request.user;
        listRequest.prefix = request.prefix;
        listRequest.pageSize = request.pageSize;
        listRequest.pageIndex = request.pageIndex;
        log_trace << "Application deleted, name: " + request.application.name;
        return ListApplications(listRequest);
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::RestartApplication(const Dto::Apps::RestartApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(LAMBDA_SERVICE_TIMER, "action", "restart_application");
        Monitoring::MetricService::instance().IncrementCounter(LAMBDA_SERVICE_COUNTER, "action", "restart_application");
        log_debug << "Restart application request, name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_warning << "Application does not exist, name: " << request.application.name;
            throw Core::ServiceException("Application does not exist, name: " + request.application.name);
        }

        // Get application
        Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);

        // Update status
        application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::PENDING);
        application = _database.UpdateApplication(application);

        // Stop the application
        if (!ContainerService::instance().ContainerExistsByName(application.containerName)) {
            log_warning << "Container does not exist, name: " << request.application.name;
            throw Core::ServiceException("Container does not exist, name: " + request.application.name);
        }

        // Stop container
        ContainerService::instance().StopContainer(application.containerId);
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

            Dto::Docker::Container container = ContainerService::instance().GetFirstContainerByImageName(application.name, application.version);
            inspectContainerResponse = ContainerService::instance().InspectContainer(container.id);
            if (inspectContainerResponse.status == http::status::ok) {
                application.imageId = inspectContainerResponse.image;
                application.containerId = inspectContainerResponse.id;
                application.containerName = inspectContainerResponse.name.substr(1);
                application.publicPort = inspectContainerResponse.hostConfig.portBindings.GetFirstPublicPort(std::to_string(application.privatePort));
                application.status = inspectContainerResponse.state.status == "running" ? Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::RUNNING) : Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
                application = _database.UpdateApplication(application);
                log_info << "Application already started, name: " << request.application.name << ", publicPort: " << application.publicPort;
            } else {
                log_error << "Could not get the status of the container, name: " << application.containerName;
            }
        }

        Dto::Apps::ListApplicationCountersRequest listRequest{};
        listRequest.requestId = request.requestId;
        listRequest.region = request.region;
        listRequest.user = request.user;
        listRequest.prefix = request.prefix;
        listRequest.pageSize = request.pageSize;
        listRequest.pageIndex = request.pageIndex;
        log_trace << "Application deleted, name: " + request.application.name;
        return ListApplications(listRequest);
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::ListApplications(const Dto::Apps::ListApplicationCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "list_applications");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "list_applications");
        log_debug << "List applications request";

        try {

            Dto::Apps::ListApplicationCountersResponse response;

            const std::vector<Database::Entity::Apps::Application> applications = _database.ListApplications(request.region);
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
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "list_application_names");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "list_application_names");
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

    Dto::Apps::ListApplicationCountersResponse ApplicationService::StopApplication(const Dto::Apps::StopApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "stop_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "stop_application");
        log_debug << "Stop application request, region:  " << request.region << " name: " << request.application.name;

        if (!_database.ApplicationExists(request.region, request.application.name)) {
            log_error << "Application does not exist, region: " << request.region << " name: " << request.application.name;
            throw Core::ServiceException("Application does not exist, region: " + request.region + " name: " + request.application.name);
        }

        try {
            Database::Entity::Apps::Application application = _database.GetApplication(request.region, request.application.name);

            if (!ContainerService::instance().ContainerExistsByName(application.containerName)) {
                log_warning << "Container does not exist, name: " << request.application.name;
                throw Core::ServiceException("Container does not exist, name: " + request.application.name);
            }

            // Stop container
            ContainerService::instance().StopContainer(application.containerName);
            application.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::STOPPED);
            application = _database.UpdateApplication(application);
            log_debug << "Application stopped, name: " << application.name;

            Dto::Apps::ListApplicationCountersRequest listRequest{};
            listRequest.requestId = request.requestId;
            listRequest.region = request.region;
            listRequest.user = request.user;
            listRequest.prefix = request.prefix;
            listRequest.pageSize = request.pageSize;
            listRequest.pageIndex = request.pageIndex;
            log_trace << "Application deleted, name: " + request.application.name;
            return ListApplications(listRequest);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Apps::ListApplicationCountersResponse ApplicationService::DeleteApplication(const Dto::Apps::DeleteApplicationRequest &request) const {
        Monitoring::MetricServiceTimer measure(APPLICATION_SERVICE_TIMER, "action", "delete_application");
        Monitoring::MetricService::instance().IncrementCounter(APPLICATION_SERVICE_COUNTER, "action", "delete_application");
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

                log_debug << "New and original are equal: " << base64FullFile;
            }
        }
        return base64FullFile;
    }

    void ApplicationService::DeleteImage(const Database::Entity::Apps::Application &application) {
        if (ContainerService::instance().ContainerExistsByName(application.containerName)) {
            ContainerService::instance().StopContainer(application.containerName);
            ContainerService::instance().DeleteContainer(application.containerName);
            log_info << "Container stopped, name: " << application.containerName;
            ContainerService::instance().DeleteImage(application.imageId);
            log_info << "Image deleted, name: " << application.name;
        }
    }
}// namespace AwsMock::Service
