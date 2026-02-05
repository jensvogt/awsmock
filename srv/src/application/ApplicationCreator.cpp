//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/apps/ApplicationCreator.h>

namespace AwsMock::Service {

    void ApplicationCreator::operator()(const std::string &applicationCodeFile, const std::string &region, const std::string &name, const std::string &instanceId) const {

        log_debug << "Start creating application, region: " << region << ", name: " << name;

        // Make a local copy
        Database::Entity::Apps::Application applicationEntity = Database::ApplicationDatabase::instance().GetApplication(region, name);

        // Create a new instance
        CreateInstance(instanceId, applicationEntity, applicationCodeFile);

        // Update database
        applicationEntity.lastStarted = system_clock::now();
        applicationEntity.status = Dto::Apps::AppsStatusTypeToString(Dto::Apps::AppsStatusType::RUNNING);
        applicationEntity = Database::ApplicationDatabase::instance().UpdateApplication(applicationEntity);

        log_info << "Application installed: " << applicationEntity.name << ", status: " << applicationEntity.status;
    }

    void ApplicationCreator::CreateInstance(const std::string &instanceId, Database::Entity::Apps::Application &applicationEntity, const std::string &applicationCodeFile) {

        // Docker tag
        applicationEntity.tags["dockerTag"] = applicationEntity.version;
        applicationEntity.tags["version"] = applicationEntity.version;
        log_debug << "Using docker tag: " << applicationEntity.version;

        // Build the docker image, if not existing
        if (!ContainerService::instance().ImageExists(applicationEntity.name, applicationEntity.version)) {
            CreateDockerImage(applicationCodeFile, applicationEntity, applicationEntity.version);
        }

        // Create the container, if not existing. If existing, get the current port from the docker container
        if (!ContainerService::instance().ContainerExistsByImageName(applicationEntity.name, applicationEntity.version)) {
            CreateDockerContainer(applicationEntity, instanceId, applicationEntity.version);
        }

        // Get docker container
        const std::string containerName = applicationEntity.name + "-" + instanceId;
        Dto::Docker::InspectContainerResponse inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);

        // Start the docker container, in case it is not already running.
        if (!inspectContainerResponse.state.running && !inspectContainerResponse.id.empty()) {
            ContainerService::instance().StartContainer(inspectContainerResponse.id);
            ContainerService::instance().WaitForContainer(inspectContainerResponse.id);
            log_info << "Application docker container started, name: " << applicationEntity.name << ", containerId: " << Core::StringUtils::Continuation(inspectContainerResponse.id, 16);
        }

        // Get the public port and set some attributes
        inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);
        if (!inspectContainerResponse.id.empty()) {
            applicationEntity.publicPort = inspectContainerResponse.hostConfig.GetFirstPublicPort(std::to_string(applicationEntity.privatePort));
            applicationEntity.containerId = inspectContainerResponse.id;
            applicationEntity.containerName = inspectContainerResponse.GetContainerName();
            applicationEntity.lastStarted = system_clock::now();
        }
    }

    void ApplicationCreator::CreateDockerImage(const std::string &applicationCodeFile, Database::Entity::Apps::Application &applicationEntity, const std::string &dockerTag) {

        log_info << "Start creating docker image, application: " << applicationEntity.name << ":" << dockerTag;

        // Check application code file
        if (!Core::FileUtils::FileExists(applicationCodeFile)) {
            log_error << "Application code file does not exist, path: " << applicationCodeFile;
            return;
        }

        // Create the code dir
        std::string codeDir = Core::DirUtils::CreateTempDir();
        log_debug << "Code directory created, codeDir: " << codeDir;

        // Read the base64 encoded zip file
        std::ifstream ifs(applicationCodeFile);
        const std::string applicationCode((std::istreambuf_iterator(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        Core::Crypto::Base64Decode(applicationCode, codeDir + Core::FileUtils::separator() + applicationEntity.archive);
        log_debug << "Created Base64 string, length: " << applicationCode.size();

        // Unzip if necessary
        if (applicationEntity.runtime.find("python") != std::string::npos) {
            Core::ZipUtils::Unzip(codeDir + Core::FileUtils::separator() + applicationEntity.archive, codeDir);
            Core::FileUtils::RemoveFile(codeDir + Core::FileUtils::separator() + applicationEntity.archive);
            log_debug << "Unzipped Python code, dir: " << codeDir;
        }

        // Build the docker image using the docker module
        const std::string imageFile = ContainerService::instance().BuildApplicationImage(codeDir, applicationEntity.name, dockerTag, applicationEntity.runtime, applicationEntity.archive, applicationEntity.privatePort, applicationEntity.environment);

        // Get the image struct
        const Dto::Docker::Image image = ContainerService::instance().GetImageByName(applicationEntity.name, dockerTag);
        applicationEntity.imageId = image.id;
        applicationEntity.imageSize = image.size;
        applicationEntity.imageMd5 = Core::Crypto::GetMd5FromFile(imageFile);

        // Cleanup
        Core::DirUtils::DeleteDirectory(codeDir);
        log_info << "Finished creating docker image, name: " << applicationEntity.name << ":" << dockerTag << ", size: " << applicationEntity.imageSize;
    }

    void ApplicationCreator::CreateDockerContainer(const Database::Entity::Apps::Application &applicationEntity, const std::string &instanceId, const std::string &dockerTag) {
        log_info << "Start creating docker container, application: " << applicationEntity.name << ", hostPort: " << applicationEntity.publicPort << ", version: " << dockerTag;
        try {

            const std::string containerName = applicationEntity.name + "-" + instanceId;
            const std::vector<std::string> environment = GetEnvironment(applicationEntity);
            const long publicPort = applicationEntity.publicPort < 0 ? Core::SystemUtils::GetNextFreePort() : applicationEntity.publicPort;
            const long privatePort = applicationEntity.privatePort;
            const Dto::Docker::CreateContainerResponse containerCreateResponse = ContainerService::instance().CreateContainer(applicationEntity.name, containerName, dockerTag, environment, publicPort, privatePort);
            log_debug << "Application container created, publicPort: " << publicPort << " containerId: " << containerCreateResponse.id;

        } catch (std::exception &exc) {
            log_error << exc.what();
        }
        log_info << "Finished creating docker container, application: " << applicationEntity.name << ", version: " << dockerTag;
    }

    std::string ApplicationCreator::UnpackZipFile(const std::string &codeDir, const std::string &applicationCode, const std::string &runtime) {

        const auto tempDir = Core::Configuration::instance().GetValue<std::string>("awsmock.temp-dir");

        // Decode the Base64 file
        const std::string zipFile = tempDir + Core::FileUtils::separator() + Core::StringUtils::GenerateRandomHexString(8) + ".zip";
        Core::Crypto::Base64Decode(applicationCode, zipFile);

        try {

            // Save zip file
            if (Core::StringUtils::ContainsIgnoreCase(runtime, "java")) {

                // Create the classes directory
                const std::string classesDir = codeDir + Core::FileUtils::separator() + "classes";
                Core::DirUtils::EnsureDirectory(classesDir);

                // Decompress, the Java JAR file to a classes' directory.
                Core::ZipUtils::Unzip(zipFile, classesDir);

            } else {

                // Decompress the Python/C/go code
                Core::ZipUtils::Unzip(zipFile, codeDir);
            }

            // Cleanup
            Core::FileUtils::RemoveFile(zipFile);

            log_debug << "ZIP file unpacked, dir: " << codeDir;
            return codeDir;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::vector<std::string> ApplicationCreator::GetEnvironment(const Database::Entity::Apps::Application &application) {

        std::vector<std::string> environment;
        environment.reserve(application.environment.size() + 1);
        for (const auto &[fst, snd]: application.environment) {
            environment.emplace_back(fst + "=" + snd);
        }
        log_debug << "Application runtime environment converted, size: " << environment.size();
        return environment;
    }

    template<typename Out>
    Out load_file(std::string const &filename, Out out) {
        std::ifstream ifs(filename, std::ios::binary);
        ifs.exceptions(std::ios::failbit | std::ios::badbit);// we prefer exceptions
        return std::copy(std::istreambuf_iterator(ifs), {}, out);
    }

}// namespace AwsMock::Service
