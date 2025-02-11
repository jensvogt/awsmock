//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/lambda/LambdaCreator.h>

namespace AwsMock::Service {

    void LambdaCreator::operator()(const std::string &functionCode, const std::string &functionId, const std::string &instanceId) const {

        log_debug << "Start creating lambda function, oid: " << functionId;

        // Make local copy
        Database::Entity::Lambda::Lambda lambdaEntity = Database::LambdaDatabase::instance().GetLambdaById(functionId);

        // Create a new instance
        CreateInstance(instanceId, lambdaEntity, functionCode);

        // Update database
        lambdaEntity.lastStarted = system_clock::now();
        lambdaEntity.state = Database::Entity::Lambda::LambdaState::Active;
        lambdaEntity.stateReason = "Activated";
        lambdaEntity.codeSize = functionCode.size();
        lambdaEntity = Database::LambdaDatabase::instance().UpdateLambda(lambdaEntity);

        log_info << "Lambda function installed: " << lambdaEntity.function << " status: " << LambdaStateToString(lambdaEntity.state);
    }

    void LambdaCreator::CreateInstance(const std::string &instanceId, Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &functionCode) {

        // Docker tag
        if (lambdaEntity.dockerTag.empty()) {
            lambdaEntity.dockerTag = GetDockerTag(lambdaEntity);
            log_debug << "Using docker tag: " << lambdaEntity.dockerTag;
        }

        // Build the docker image, if not existing
        if (!ContainerService::instance().ImageExists(lambdaEntity.function, lambdaEntity.dockerTag)) {
            CreateDockerImage(functionCode, lambdaEntity, lambdaEntity.dockerTag);
        }

        // Create the container, if not existing. If existing get the current port from the docker container
        Database::Entity::Lambda::Instance instance;
        const std::string containerName = lambdaEntity.function + "-" + instanceId;
        if (!ContainerService::instance().ContainerExistsByName(containerName)) {
            CreateDockerContainer(lambdaEntity, instanceId, CreateRandomHostPort(), lambdaEntity.dockerTag);
        }

        // Get docker container
        Dto::Docker::Container container = ContainerService::instance().GetContainerById(containerName);
        Dto::Docker::InspectContainerResponse inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);

        // Start docker container, in case it is not already running.
        if (!inspectContainerResponse.state.running) {
            ContainerService::instance().StartDockerContainer(inspectContainerResponse.id);
            ContainerService::instance().WaitForContainer(inspectContainerResponse.id);
            log_debug << "Lambda docker container started, containerId: " << inspectContainerResponse.id;
        }

        // Get the public port
        inspectContainerResponse = ContainerService::instance().InspectContainer(containerName);
        instance.instanceId = instanceId;
        instance.hostPort = inspectContainerResponse.hostConfig.portBindings.GetFirstPublicPort(8080);
        instance.status = Database::Entity::Lambda::InstanceIdle;
        instance.containerId = inspectContainerResponse.id;
        instance.containerName = containerName;
        instance.created = system_clock::now();
        lambdaEntity.instances.emplace_back(instance);

        // Save size in entity
        lambdaEntity.containerSize = container.sizeRootFs;
    }

    void LambdaCreator::CreateDockerImage(const std::string &functionCode, Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &dockerTag) {

        std::string codeDir = Core::DirUtils::CreateTempDir("/tmp");
        log_debug << "Code directory created, codeDir: " << codeDir;

        // Write base64 encoded zip file
        const std::string encodedFile = WriteBase64File(functionCode, lambdaEntity, dockerTag);
        log_debug << "Create Base64 string, length: " << functionCode.size();

        // Unzip provided zip-file into a temporary directory
        codeDir = UnpackZipFile(codeDir, functionCode, lambdaEntity.runtime);
        log_debug << "Lambda file unzipped, codeDir: " << codeDir;

        // Build the docker image using the docker module
        const std::string imageFile = ContainerService::instance().BuildImage(codeDir, lambdaEntity.function, dockerTag, lambdaEntity.handler, lambdaEntity.runtime, lambdaEntity.environment.variables);

        // Get the image struct
        const Dto::Docker::Image image = ContainerService::instance().GetImageByName(lambdaEntity.function, dockerTag);
        lambdaEntity.imageId = image.id;
        lambdaEntity.imageSize = image.size;
        lambdaEntity.codeSha256 = Core::Crypto::GetSha256FromFile(imageFile);

        // Cleanup
        Core::DirUtils::DeleteDirectory(codeDir);
        log_debug << "Docker image created, name: " << lambdaEntity.function << " size: " << lambdaEntity.codeSize;
    }

    void LambdaCreator::CreateDockerContainer(const Database::Entity::Lambda::Lambda &lambdaEntity, const std::string &instanceId, const int hostPort, const std::string &dockerTag) {

        try {

            const std::string containerName = lambdaEntity.function + "-" + instanceId;
            const std::vector<std::string> environment = GetEnvironment(lambdaEntity);
            const Dto::Docker::CreateContainerResponse containerCreateResponse = ContainerService::instance().CreateContainer(lambdaEntity.function, containerName, dockerTag, environment, hostPort);
            log_debug << "Lambda container created, hostPort: " << hostPort << " containerId: " << containerCreateResponse.id;

        } catch (std::exception &exc) {
            log_error << exc.what();
        }
    }

    std::string LambdaCreator::UnpackZipFile(const std::string &codeDir, const std::string &functionCode, const std::string &runtime) {

        std::string dataDir = Core::Configuration::instance().GetValueString("awsmock.data-dir");
        std::string tempDir = Core::Configuration::instance().GetValueString("awsmock.temp-dir");

        // Decode Base64 file
        std::string decoded = Core::Crypto::Base64Decode(functionCode);
        std::string zipFile = tempDir + "/zipfile.zip";
        try {

            // Write to temp file
            std::ofstream ofs(zipFile);
            ofs << decoded;
            ofs.close();
            decoded.clear();

            // Save zip file
            if (Core::StringUtils::ContainsIgnoreCase(runtime, "java")) {

                // Create classes directory
                std::string classesDir = codeDir + Core::FileUtils::separator() + "classes";
                Core::DirUtils::EnsureDirectory(classesDir);

                // Decompress, the Java JAR file to a classes' directory.
                Core::TarUtils::Unzip(zipFile, classesDir);

            } else {

                // Decompress the Python/C/go code
                Core::TarUtils::Unzip(zipFile, codeDir);
            }
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

    int LambdaCreator::CreateRandomHostPort() {
        return Core::RandomUtils::NextInt(HOST_PORT_MIN, HOST_PORT_MAX);
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

        std::string s3DataDir = Core::Configuration::instance().GetValueString("awsmock.modules.s3.data-dir");
        std::string lambdaDir = Core::Configuration::instance().GetValueString("awsmock.modules.lambda.data-dir");

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
        base64EncodedCodeString.clear();

        lambda.code.zipFile = base64File;
        return base64FullFile;
    }
}// namespace AwsMock::Service
