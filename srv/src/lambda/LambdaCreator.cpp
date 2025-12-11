//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/lambda/LambdaCreator.h>

namespace AwsMock::Service {

    Database::Entity::Lambda::Lambda LambdaCreator::CreateLambda(Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId) const {
        log_debug << "Start creating lambda function, instanceId: " << instanceId;

        // Create a new instance
        const std::string containerId = CreateInstance(instanceId, lambda, lambda.code.zipFile);

        // Update database
        lambda.averageRuntime = 0;
        lambda.invocations = 0;
        lambda.lastStarted = system_clock::now();
        lambda.state = Database::Entity::Lambda::LambdaState::Active;
        lambda.stateReason = "Activated";
        lambda.codeSize = static_cast<long>(lambda.code.zipFile.size());
        lambda = _lambdaDatabase.UpdateLambda(lambda);

        log_info << "Lambda function instance created: " << lambda.function << " instanceId: " << instanceId << ", instances: " << lambda.instances.size();
        return lambda;
    }

    std::string LambdaCreator::CreateInstance(const std::string &instanceId, Database::Entity::Lambda::Lambda &lambda, const std::string &functionCode) const {

        const auto privatePort = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.private-port");

        // Docker tag
        if (lambda.dockerTag.empty()) {
            lambda.dockerTag = GetDockerTag(lambda);
            lambda.tags["dockerTag"] = lambda.dockerTag;
            lambda.tags["version"] = lambda.dockerTag;
            log_debug << "Using docker tag: " << lambda.dockerTag;
        }

        // Build the docker image, if not existing
        if (!ContainerService::instance().ImageExists(lambda.function, lambda.dockerTag)) {
            CreateDockerImage(functionCode, lambda, lambda.dockerTag);
        }

        // Create the container, if not existing. If existing, get the current port from the docker container
        const int hostPort = Core::SystemUtils::GetNextFreePort();
        const std::string containerName = lambda.function + "-" + instanceId;
        if (!ContainerService::instance().ContainerExists(containerName)) {
            CreateDockerContainer(lambda, instanceId, hostPort, lambda.dockerTag);
        }

        // Get docker container
        Dto::Docker::InspectContainerResponse inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);

        // Start the docker container, in case it is not already running.
        if (!inspectContainerResponse.state.running && !inspectContainerResponse.id.empty()) {
            ContainerService::instance().StartContainer(inspectContainerResponse.id);
            ContainerService::instance().WaitForContainer(inspectContainerResponse.id);
            log_info << "Lambda docker container started, function: " << lambda.function << ", containerId: " << inspectContainerResponse.id;
        }

        // Get the public port
        inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);
        Database::Entity::Lambda::Instance instance;
        instance.instanceId = instanceId;
        instance.status = Database::Entity::Lambda::InstanceIdle;
        instance.containerName = containerName;
        instance.created = system_clock::now();
        if (!inspectContainerResponse.id.empty()) {
            instance.hostName = _dockerized ? containerName : std::string("localhost");
            instance.hostPort = _dockerized ? 8080 : hostPort;
            instance.containerId = inspectContainerResponse.id;
            lambda.containerSize = inspectContainerResponse.sizeRootFs;
        }
        lambda.instances.emplace_back(instance);

        // Save size in entity
        return inspectContainerResponse.id;
    }

    void LambdaCreator::UpdateLambda(Database::Entity::Lambda::Lambda &lambda, const std::string &functionCode, const std::string &newVersion) const {

        const auto privatePort = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.private-port");
        const auto dockerized = Core::Configuration::instance().GetValue<bool>("awsmock.dockerized");

        // Docker tag
        lambda.dockerTag = newVersion;
        lambda.tags["dockerTag"] = lambda.dockerTag;
        lambda.tags["version"] = lambda.dockerTag;
        log_debug << "Using docker tag: " << lambda.dockerTag;

        // Build the docker image, if not existing
        CreateDockerImage(functionCode, lambda, lambda.dockerTag);

        // Create instance
        const std::string instanceId = Core::StringUtils::GenerateRandomHexString(8);

        // Set public port

        // Create the container, if not existing. If existing, get the current port from the docker container
        const std::string containerName = lambda.function + "-" + instanceId;
        CreateDockerContainer(lambda, instanceId, Core::SystemUtils::GetNextFreePort(), lambda.dockerTag);

        // Get docker container
        Dto::Docker::InspectContainerResponse inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);

        // Start the docker container, in case it is not already running.
        if (!inspectContainerResponse.state.running && !inspectContainerResponse.id.empty()) {
            ContainerService::instance().StartContainer(inspectContainerResponse.id);
            ContainerService::instance().WaitForContainer(inspectContainerResponse.id);
            log_info << "Lambda docker container started, function: " << lambda.function << ", containerId: " << inspectContainerResponse.id;
        }

        // Get the public port
        inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);
        Database::Entity::Lambda::Instance instance;
        instance.instanceId = instanceId;
        instance.status = Database::Entity::Lambda::InstanceIdle;
        instance.containerName = containerName;
        instance.created = system_clock::now();
        if (!inspectContainerResponse.id.empty()) {
            instance.hostName = dockerized ? containerName : "localhost";
            instance.hostPort = inspectContainerResponse.hostConfig.GetFirstPublicPort(privatePort);
            instance.containerId = inspectContainerResponse.id;
            lambda.containerSize = inspectContainerResponse.sizeRootFs;
        }
        lambda.instances.emplace_back(instance);

        // Set status and update database
        lambda.state = Database::Entity::Lambda::LambdaState::Active;
        lambda = _lambdaDatabase.UpdateLambda(lambda);
    }

    void LambdaCreator::CreateDockerImage(const std::string &zipFile, Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &dockerTag) {

        log_info << "Start creating docker image, name: " << lambdaEntity.function << ":" << dockerTag;

        std::string codeDir = Core::DirUtils::CreateTempDir();
        log_debug << "Code directory created, codeDir: " << codeDir;

        // Check the base64 encoded zip file
        const auto lambdaDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.data-dir");
        const std::string base64FullFile = lambdaDir + Core::FileUtils::separator() + zipFile;
        if (!Core::FileUtils::FileExists(base64FullFile)) {
            log_error << "Base64 file does not exist, path: " << base64FullFile;
            return;
        }

        // Read the function code
        const std::string functionCode = Core::FileUtils::ReadFile(base64FullFile);
        log_debug << "Created Base64 string, length: " << functionCode.size();

        // Unzip provided zip-file into a temporary directory
        codeDir = UnpackZipFile(codeDir, functionCode, lambdaEntity.runtime);
        log_debug << "Lambda file unzipped, codeDir: " << codeDir;

        // Build the docker image using the docker module
        const std::string imageFile = ContainerService::instance().BuildLambdaImage(codeDir, lambdaEntity.function, dockerTag, lambdaEntity.handler, lambdaEntity.runtime, lambdaEntity.environment.variables);

        // Get the image struct
        const Dto::Docker::Image image = ContainerService::instance().GetImageByName(lambdaEntity.function, dockerTag);
        lambdaEntity.imageId = image.id;
        lambdaEntity.imageSize = image.size;
        lambdaEntity.codeSha256 = Core::Crypto::GetSha256FromFile(imageFile);

        // Cleanup
        Core::DirUtils::DeleteDirectory(codeDir);
        log_info << "Finished creating docker image, name: " << lambdaEntity.function << " size: " << std::to_string(lambdaEntity.codeSize) + ", codeDir: " << codeDir;
    }

    void LambdaCreator::CreateDockerContainer(const Database::Entity::Lambda::Lambda &lambda, const std::string &instanceId, const int hostPort, const std::string &dockerTag) {
        log_info << "Creating docker container, function: " << lambda.function << " hostPort: " << hostPort << " dockerTag: " << dockerTag;
        try {

            const std::string containerName = lambda.function + "-" + instanceId;
            const std::vector<std::string> environment = GetEnvironment(lambda);
            const Dto::Docker::CreateContainerResponse containerCreateResponse = ContainerService::instance().CreateContainer(lambda.function, containerName, dockerTag, environment, hostPort);
            log_debug << "Lambda container created, hostPort: " << hostPort << " containerId: " << containerCreateResponse.id;

        } catch (std::exception &exc) {
            log_error << exc.what();
        }
    }

    std::string LambdaCreator::UnpackZipFile(const std::string &codeDir, const std::string &functionCode, const std::string &runtime) {

        const auto tempDir = Core::Configuration::instance().GetValue<std::string>("awsmock.temp-dir");

        // Decode the Base64 file
        const std::string zipFile = tempDir + Core::FileUtils::separator() + Core::StringUtils::GenerateRandomHexString(8) + ".zip";
        Core::Crypto::Base64Decode(functionCode, zipFile);

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

    std::vector<std::string> LambdaCreator::GetEnvironment(const Database::Entity::Lambda::Lambda &lambda) {

        std::vector<std::string> environment;
        environment.reserve(lambda.environment.variables.size() + 1);
        for (const auto &[fst, snd]: lambda.environment.variables) {
            environment.emplace_back(fst + "=" + snd);
        }
        environment.emplace_back("AWS_LAMBDA_FUNCTION_TIMEOUT=" + std::to_string(lambda.timeout));
        log_debug << "lambda runtime environment converted, size: " << environment.size();
        return environment;
    }

    std::string LambdaCreator::GetDockerTag(const Database::Entity::Lambda::Lambda &lambda) {
        if (lambda.HasTag("version")) {
            return lambda.GetTagValue("version");
        }
        if (lambda.HasTag("dockerTag")) {
            return lambda.GetTagValue("dockerTag");
        }
        if (lambda.HasTag("tag")) {
            return lambda.GetTagValue("tag");
        }
        return "latest";
    }

    template<typename Out>
    Out load_file(std::string const &filename, Out out) {
        std::ifstream ifs(filename, std::ios::binary);
        ifs.exceptions(std::ios::failbit | std::ios::badbit);// we prefer exceptions
        return std::copy(std::istreambuf_iterator(ifs), {}, out);
    }

    std::string LambdaCreator::WriteBase64File(const std::string &zipFile, Database::Entity::Lambda::Lambda &lambda, const std::string &dockerTag) {

        auto s3DataDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.s3.data-dir");
        auto lambdaDir = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.lambda.data-dir");

        std::string base64File = lambda.function + "-" + dockerTag + ".b64";
        std::string base64FullFile = lambdaDir + Core::FileUtils::separator() + base64File;
        log_debug << "Using Base64File: " << base64FullFile;

        std::string base64EncodedCodeString = zipFile;

        // Write base64 string, either from S3 bucket/key or from supplied string
        if (zipFile.empty() && !lambda.code.s3Bucket.empty() && !lambda.code.s3Key.empty()) {

            // Get internal name of S3 object
            Database::Entity::S3::Object s3Object = Database::S3Database::instance().GetObject(lambda.region, lambda.code.s3Bucket, lambda.code.s3Key);
            std::string s3CodeFile = s3DataDir + Core::FileUtils::separator() + s3Object.internalName;

            // Load file
            std::vector<char> input;
            load_file(s3CodeFile, back_inserter(input));

            // Allocate "enough" space, using an upperbound prediction:
            std::string encoded(boost::beast::detail::base64::encoded_size(input.size()), '\0');

            // Encode returns the actual encoded_size
            auto encoded_size = boost::beast::detail::base64::encode(encoded.data(), input.data(), input.size());
            encoded.resize(encoded_size);

            base64EncodedCodeString = encoded;
        }

        // If we do not have a local file already or the MD5 sum changed, write the Base64 encoded file to lambda dir
        if (!Core::FileUtils::FileExists(base64FullFile)) {

            std::ofstream ofs(base64FullFile);
            ofs << base64EncodedCodeString;
            ofs.close();
            log_debug << "New Base64 file written: " << base64FullFile;

        } else {

            std::string md5sumFile = Core::Crypto::GetMd5FromFile(base64FullFile);
            if (std::string md5sumString = Core::Crypto::GetMd5FromString(base64EncodedCodeString); md5sumFile != md5sumString) {

                std::ofstream ofs(base64FullFile);
                ofs << base64EncodedCodeString;
                ofs.close();
                log_debug << "Updated Base64 file written: " << base64FullFile;

            } else {
                log_debug << "New and original are equal: " << base64FullFile;
            }
        }
        lambda.code.zipFile = base64File;
        return base64FullFile;
    }
}// namespace AwsMock::Service
