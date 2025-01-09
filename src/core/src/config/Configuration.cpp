//
// Created by vogje01 on 22/08/2022.
//

#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {

    Configuration::Configuration() {
        Initialize();
    }

    Configuration::Configuration(const std::string &basename) : _yamlConfig(YAML::Null) {
        Initialize();
        SetFilename(basename);
    }

    void Configuration::Initialize() {
        // General
        DefineStringProperty("awsmock.region", "AWSMOCK_REGION", "eu-central-1");
        DefineStringProperty("awsmock.user", "AWSMOCK_USER", "none");
        DefineStringProperty("awsmock.access.key-id", "AWSMOCK_ACCESS_ACCOUNT_KEY_ID", "none");
        DefineStringProperty("awsmock.access.account-id", "AWSMOCK_ACCESS_ACCOUNT_ID", "000000000000");
        DefineStringProperty("awsmock.access.client-id", "AWSMOCK_ACCESS_CLIENT_ID", "00000000");
        DefineStringProperty("awsmock.access.secret-access-key", "AWSMOCK_ACCESS_SECRET_ACCESS_KEY", "none");
        DefineStringProperty("awsmock.data-dir", "AWSMOCK_DATA_DIR", "/tmp/awsmock/data");
        DefineStringProperty("awsmock.temp-dir", "AWSMOCK_TEMP_DIR", "/tmp/awsmock/tmp");
        DefineBoolProperty("awsmock.json.pretty", "AWSMOCK_PRETTY", false);
        DefineBoolProperty("awsmock.aws.signature.verify", "AWSMOCK_VERIFY_SIGNATURE", false);
        DefineBoolProperty("awsmock.dockerized", "AWSMOCK_DOCKERIZED", false);

        // Gateway
        DefineBoolProperty("awsmock.gateway.active", "AWSMOCK_GATEWAY_ACTIVE", true);
        DefineStringProperty("awsmock.gateway.http.host", "AWSMOCK_GATEWAY_HOST", "localhost");
        DefineStringProperty("awsmock.gateway.http.address", "AWSMOCK_GATEWAY_ADDRESS", "0.0.0.0");
        DefineIntProperty("awsmock.gateway.http.port", "AWSMOCK_GATEWAY_PORT", 4566);
        DefineIntProperty("awsmock.gateway.http.max-queue", "AWSMOCK_GATEWAY_MAX_QUEUE", 10);
        DefineIntProperty("awsmock.gateway.http.max-threads", "AWSMOCK_GATEWAY_MAX_THREADS", 50);
        DefineIntProperty("awsmock.gateway.http.max-body", "AWSMOCK_GATEWAY_MAX_BODY", 104857600);
        DefineIntProperty("awsmock.gateway.http.timeout", "AWSMOCK_GATEWAY_TIMEOUT", 900);

        // S3
        DefineBoolProperty("awsmock.modules.s3.active", "AWSMOCK_MODULES_S3_ACTIVE", true);
        DefineStringProperty("awsmock.modules.s3.data-dir", "AWSMOCK_MODULES_S3_DATA_DIR", "/tmp/awsmock/data/s3");
        DefineIntProperty("awsmock.modules.s3.monitoring.period", "AWSMOCK_MODULES_S3_MONITORING_PERIOD", 900);
        DefineIntProperty("awsmock.modules.s3.sync.object.period", "AWSMOCK_MODULES_S3_SYNC_OBJECT_PERIOD", 3600);
        DefineIntProperty("awsmock.modules.s3.sync.bucket.period", "AWSMOCK_MODULES_S3_SYNC_BUCKET_PERIOD", 300);

        // SQS
        DefineBoolProperty("awsmock.modules.sqs.active", "AWSMOCK_MODULES_SQS_ACTIVE", true);
        DefineIntProperty("awsmock.modules.s3.monitoring.period", "AWSMOCK_MONITORING_SQS_PERIOD", 300);
        DefineIntProperty("awsmock.modules.s3.reset.period", "AWSMOCK_WORKER_SQS_RESET_PERIOD", 30);
        DefineIntProperty("awsmock.modules.s3.counter.period", "AWSMOCK_WORKER_SQS_COUNTER_PERIOD", 30);

        // SNS
        DefineBoolProperty("awsmock.modules.sns.active", "AWSMOCK_MODULES_SNS_ACTIVE", true);
        DefineIntProperty("awsmock.modules.sns.timeout", "AWSMOCK_MODULES_SNS_TIMEOUT", 14);
        DefineIntProperty("awsmock.monitoring.sns.monitoring.period", "AWSMOCK_SNS_MONITORING_PERIOD", 300);
        DefineIntProperty("awsmock.monitoring.sns.delete.period", "AWSMOCK_SNS_DELETE_PERIOD", 300);
        DefineIntProperty("awsmock.monitoring.sns.counter.period", "AWSMOCK_SNS_COUNTER_PERIOD", 300);

        // Lambda
        DefineBoolProperty("awsmock.modules.lambda.active", "AWSMOCK_MODULES_LAMBDA_ACTIVE", true);
        DefineIntProperty("awsmock.modules.lambda.lifetime", "AWSMOCK_MODULES_LAMBDA_LIFETIME", 3600);
        DefineStringProperty("awsmock.modules.lambda.data-dir", "AWSMOCK_MODULES_LAMBDA_DATADIR", "/home/awsmock/data/lambda");
        DefineIntProperty("awsmock.modules.lambda.monitoring.period", "AWSMOCK_MODULES_LAMBDA_MONITORING_PERIOD", 300);
        DefineIntProperty("awsmock.modules.lambda.remove.period", "AWSMOCK_MODULES_LAMBDA_REMOVE_PERIOD", 300);
        DefineIntProperty("awsmock.modules.lambda.counter.period", "AWSMOCK_MODULES_LAMBDA_COUNTER_PERIOD", 300);

        // Transfer server
        DefineBoolProperty("awsmock.modules.transfer.active", "AWSMOCK_MODULES_TRANSFER_ACTIVE", true);
        DefineStringProperty("awsmock.modules.transfer.bucket", "AWSMOCK_MODULES_TRANSFER_BUCKET", "transfer-server");
        DefineStringProperty("awsmock.modules.transfer.data-dir", "AWSMOCK_MODULES_TRANSFER_DATA_DIR", "/tmp/awsmock/data/transfer");
        DefineIntProperty("awsmock.modules.transfer.monitoring.period", "AWSMOCK_MODULES_TRANSFER_MONITORING_PERIOD", 300);
        DefineIntProperty("awsmock.modules.transfer.worker.period", "AWSMOCK_MODULES_TRANSFER_WORKER_PERIOD", 300);
        DefineIntProperty("awsmock.modules.transfer.ftp.pasv-min", "AWSMOCK_MODULES_TRANSFER_FTP_PASV_MIN", 6000);
        DefineIntProperty("awsmock.modules.transfer.ftp.pasv-max", "AWSMOCK_MODULES_TRANSFER_FTP_PASV_MAX", 6100);
        DefineIntProperty("awsmock.modules.transfer.ftp.port", "AWSMOCK_MODULES_TRANSFER_FTP_PORT", 21);
        DefineStringProperty("awsmock.modules.transfer.ftp.address", "AWSMOCK_MODULES_TRANSFER_FTP_ADDRESS", "0.0.0.0");

        // Cognito
        DefineBoolProperty("awsmock.modules.cognito.active", "AWSMOCK_MODULES_COGNITO_ACTIVE", true);
        DefineIntProperty("awsmock.modules.cognito.monitoring.period", "AWSMOCK_MODULES_COGNITO_MONITORING_PERIOD", 300);

        // DynamoDB
        DefineBoolProperty("awsmock.modules.dynamodb.active", "AWSMOCK_MODULES_DYNAMODB_ACTIVE", true);
        DefineIntProperty("awsmock.modules.dynamodb.monitoring.period", "AWSMOCK_MONITORING_DYNAMODB_PERIOD", 300);
        DefineIntProperty("awsmock.modules.dynamodb.worker.period", "AWSMOCK_MONITORING_DYNAMODB_PERIOD", 300);
        DefineStringProperty("awsmock.modules.dynamodb.container.host", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_HOST", "localhost");
        DefineIntProperty("awsmock.modules.dynamodb.container.port", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_PORT", 8000);
        DefineStringProperty("awsmock.modules.dynamodb.container.name", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_NAME", "dynamodb-local");
        DefineStringProperty("awsmock.modules.dynamodb.container.image-name", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_IMAGE_NAME", "docker.io/library/dynamodb-local");
        DefineStringProperty("awsmock.modules.dynamodb.container.image-tag", "AWSMOCK_MODULES_DYNAMODB_CONTAINER_IMAGE_TAG", "latest");

        // SecretsManager
        DefineBoolProperty("awsmock.modules.secretsmanager.active", "AWSMOCK_MODULES_SECRETSMANAGER_ACTIVE", true);
        DefineIntProperty("awsmock.modules.secretsmanager.monitoring.period", "AWSMOCK_MODULES_SECRETSMANAGER_MONITORING_PERIOD", 300);
        DefineIntProperty("awsmock.modules.secretsmanager.worker.period", "AWSMOCK_MODULES_SECRETSMANAGER_WORKER_PERIOD", 300);

        // KMS
        DefineBoolProperty("awsmock.modules.kms.active", "AWSMOCK_MODULES_KMS_ACTIVE", true);
        DefineIntProperty("awsmock.modules.kms.monitoring.period", "AWSMOCK_SERVICEK_KMS_MONITORING_PERIOD", 300);
        DefineIntProperty("awsmock.modules.kms.remove.period", "AWSMOCK_WORKER_KMS_REMOVE_PERIOD", 300);

        // Docker
        DefineBoolProperty("awsmock.docker.active", "AWSMOCK_DOCKER_ACTIVE", false);
        DefineStringProperty("awsmock.docker.network-mode", "AWSMOCK_DOCKER_NETWORK_MODE", "local");
        DefineStringProperty("awsmock.docker.network-name", "AWSMOCK_DOCKER_NETWORK_NAME", "local");
        DefineIntProperty("awsmock.docker.default.memory-size", "AWSMOCK_DOCKER_DEFAULT_MEMORY_SIZE", 512);
        DefineIntProperty("awsmock.docker.default.temp-size", "AWSMOCK_DOCKER_DEFAULT_TEMP_SIZE", 10240);
        DefineIntProperty("awsmock.docker.container.port", "AWSMOCK_DOCKER_CONTAINER_PORT", 8080);
        DefineStringProperty("awsmock.docker.socket", "AWSMOCK_DOCKER_SOCKET", "/var/run/docker.sock");
        DefineIntProperty("awsmock.docker.container.maxWaitTime", "AWSMOCK_DOCKER_CONTAINER_MAX_WAIT_TIME", 5);
        DefineIntProperty("awsmock.docker.container.checkTime", "AWSMOCK_DOCKER_CONTAINER_CHECK_TIME", 500);

        // Podman
        DefineBoolProperty("awsmock.podman.active", "AWSMOCK_PODMAN_ACTIVE", true);
        DefineStringProperty("awsmock.podman.network-mode", "AWSMOCK_PODMAN_NETWORK_MODE", "local");
        DefineStringProperty("awsmock.podman.network-name", "AWSMOCK_PODMAN_NETWORK_NAME", "local");
        DefineIntProperty("awsmock.podman.default.memory-size", "AWSMOCK_PODMAN_DEFAULT_MEMORY_SIZE", 512);
        DefineIntProperty("awsmock.podman.default.temp-size", "AWSMOCK_PODMAN_DEFAULT_TEMP_SIZE", 10240);
        DefineIntProperty("awsmock.podman.container.port", "AWSMOCK_PODMAN_CONTAINER_PORT", 8080);
        DefineStringProperty("awsmock.podman.socket", "AWSMOCK_PODMAN_SOCKET", "/run/podman/podman.sock");

        // Monitoring
        DefineIntProperty("awsmock.monitoring.port", "AWSMOCK_MONITORING_PORT", 9091);
        DefineIntProperty("awsmock.monitoring.period", "AWSMOCK_MONITORING_PERIOD", 60);
        DefineBoolProperty("awsmock.monitoring.prometheus", "AWSMOCK_MONITORING_PROMETHEUS", true);
        DefineBoolProperty("awsmock.monitoring.intern", "AWSMOCK_MONITORING_INTERN", true);
        DefineIntProperty("awsmock.monitoring.retention", "AWSMOCK_MONITORING_RETENTION", 3);

        // Database
        DefineBoolProperty("awsmock.mongodb.active", "AWSMOCK_MONGODB_ACTIVE", true);
        DefineStringProperty("awsmock.mongodb.name", "AWSMOCK_MONGODB_NAME", "awsmock");
        DefineStringProperty("awsmock.mongodb.host", "AWSMOCK_MONGODB_HOST", "localhost");
        DefineIntProperty("awsmock.mongodb.port", "AWSMOCK_MONGODB_PORT", 27017);
        DefineStringProperty("awsmock.mongodb.user", "AWSMOCK_MONGODB_USER", "root");
        DefineStringProperty("awsmock.mongodb.password", "AWSMOCK_MONGODB_PASSWORD", "secret");
        DefineIntProperty("awsmock.mongodb.pool-size", "AWSMOCK_MONGODB_POOL_SIZE", 256);

        // Frontend
        DefineBoolProperty("awsmock.frontend.active", "AWSMOCK_FRONTEND_ACTIVE", true);
        DefineStringProperty("awsmock.frontend.address", "AWSMOCK_FRONTEND_ADDRESS", "0.0.0.0");
        DefineIntProperty("awsmock.frontend.port", "AWSMOCK_FRONTEND_PORT", 4567);
        DefineIntProperty("awsmock.frontend.workers", "AWSMOCK_FRONTEND_WORKERS", 10);
        DefineIntProperty("awsmock.frontend.timeout", "AWSMOCK_FRONTEND_TIMEOUT", 900);
        DefineStringProperty("awsmock.frontend.doc-root", "AWSMOCK_FRONTEND_DOC_ROOT", "/home/awsmock/frontend");

        // Logging
        DefineStringProperty("awsmock.logging.level", "AWSMOCK_LOG_LEVEL", "info");
        DefineStringProperty("awsmock.logging.file", "AWSMOCK_LOG_FILE", "/var/run/awsmock.log");

        // Debug
        log_debug << "Default configuration defined, config: " << _yamlConfig;
    }

    void Configuration::DefineStringProperty(const std::string &key, const std::string &envProperty, const std::string &defaultValue) {
        std::string value = defaultValue;
        if (getenv(envProperty.c_str()) != nullptr) {
            value = getenv(envProperty.c_str());
            AddToEnvList(key, getenv(envProperty.c_str()));
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    void Configuration::DefineBoolProperty(const std::string &key, const std::string &envProperty, const bool defaultValue) {
        bool value = defaultValue;
        if (getenv(envProperty.c_str()) != nullptr) {
            value = getenv(envProperty.c_str()) == "true";
            AddToEnvList(key, getenv(envProperty.c_str()));
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    void Configuration::DefineIntProperty(const std::string &key, const std::string &envProperty, const int defaultValue) {
        std::string value = std::to_string(defaultValue);
        if (getenv(envProperty.c_str()) != nullptr) {
            value = getenv(envProperty.c_str());
            AddToEnvList(key, getenv(envProperty.c_str()));
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    void Configuration::DefineLongProperty(const std::string &key, const std::string &envProperty, const long defaultValue) {
        long value = defaultValue;
        if (getenv(envProperty.c_str()) != nullptr) {
            value = std::stol(getenv(envProperty.c_str()));
            AddToEnvList(key, getenv(envProperty.c_str()));
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    void Configuration::DefineDoubleProperty(const std::string &key, const std::string &envProperty, const double defaultValue) {
        double value = defaultValue;
        if (getenv(envProperty.c_str()) != nullptr) {
            value = std::stod(getenv(envProperty.c_str()));
            AddToEnvList(key, getenv(envProperty.c_str()));
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Defined property, key: " << key << " property: " << envProperty << " default: " << defaultValue;
    }

    std::string Configuration::GetFilename() const {
        if (_filename.empty()) {
            throw CoreException("Filename not set");
        }
        return _filename;
    }

    void Configuration::SetFilename(const std::string &filename) {
        if (filename.empty()) {
            throw CoreException("Empty filename");
        }
        if (!FileUtils::FileExists(filename)) {
            log_warning << "Configuration file '" << filename << "' does not exist. Will use default.";
        }

        // Save file name
        _filename = filename;

        // Parse YAML file
        _yamlConfig = YAML::LoadFile(filename);

        // Reapply environment settings
        ApplyEnvSettings();

        log_debug << ToString();
    }

    void Configuration::SetValueString(const std::string &key, const std::string &value) {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " + key;
            throw CoreException("Property not found, key: " + key);
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Value set, key: " << key;
    }

    void Configuration::SetValueBool(const std::string &key, const bool value) {
        if (!HasProperty(key)) {
            throw CoreException("Property not found, key: " + key);
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Value set, key: " << key;
    }

    void Configuration::SetValueInt(const std::string &key, const int value) {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " + key;
            throw CoreException("Property not found, key: " + key);
        }
        SetValueByPath(_yamlConfig, key, value);
        log_trace << "Value set, key: " << key;
    }

    std::string Configuration::GetValueString(const std::string &key) const {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " + key;
            throw CoreException("Property not found, key: " + key);
        }
        std::vector<std::string> paths = StringUtils::Split(key, '.');
        return lookup(_yamlConfig, paths.begin(), paths.end()).as<std::string>();
    }

    int Configuration::GetValueInt(const std::string &key) const {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " + key;
            throw CoreException("Property not found, key: " + key);
        }
        std::vector<std::string> paths = StringUtils::Split(key, '.');
        return lookup(_yamlConfig, paths.begin(), paths.end()).as<int>();
    }

    long Configuration::GetValueLong(const std::string &key) const {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " + key;
            throw CoreException("Property not found, key: " + key);
        }
        std::vector<std::string> paths = StringUtils::Split(key, '.');
        return lookup(_yamlConfig, paths.begin(), paths.end()).as<long>();
    }

    bool Configuration::GetValueBool(const std::string &key) const {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " + key;
            throw CoreException("Property not found, key: " + key);
        }
        std::vector<std::string> paths = StringUtils::Split(key, '.');
        return lookup(_yamlConfig, paths.begin(), paths.end()).as<bool>();
    }

    double Configuration::GetValueDouble(const std::string &key) const {
        if (!HasProperty(key)) {
            log_error << "Property not found, key: " + key;
            throw CoreException("Property not found, key: " + key);
        }
        std::vector<std::string> paths = StringUtils::Split(key, '.');
        return lookup(_yamlConfig, paths.begin(), paths.end()).as<double>();
    }

    std::string Configuration::GetAppName() {
        return PROJECT_NAME;
    }

    std::string Configuration::GetVersion() {
        return PROJECT_VERSION;
    }

    std::string Configuration::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    void Configuration::Dump() const {
        std::cerr << _yamlConfig << std::endl;
    }

    void Configuration::WriteFile(const std::string &filename) const {
        std::ofstream ofs(filename);
        ofs << _yamlConfig;
        ofs.close();
    }

    void Configuration::AddToEnvList(const std::string &key, const std::string &value) {
        _envList[key] = value;
    }

    void Configuration::ApplyEnvSettings() {
        for (const auto &[fst, snd]: _envList) {
            SetValueString(fst, snd);
        }
    }

    bool Configuration::HasProperty(const std::string &key) const {
        std::vector<std::string> paths = StringUtils::Split(key, '.');
        YAML::Node traverse = _yamlConfig;
        for (std::string &path_element: paths) {
            traverse.reset(traverse[path_element]);
        }
        return traverse.IsDefined();
    }

    std::ostream &operator<<(std::ostream &os, const Configuration &s) {
        os << "Configuration={" + s.ToString() + "}";
        return os;
    }
}// namespace AwsMock::Core
