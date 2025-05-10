//
// Created by vogje01 on 23/07/2023.
//

#include <awsmock/core/TestUtils.h>

namespace AwsMock::Core {

    void TestUtils::CreateTestConfigurationFile(const bool withDatabase) {

        LogStream::Initialize();
        LogStream::SetSeverity("info");

        // Logging
        if (!SystemUtils::GetEnvironmentVariableValue("AWSMOCK_TEST_LOG").empty()) {
            LogStream::AddFile("/tmp", "awsmocktest", 1024 * 1024, 5);
        }

        constexpr int port = 14566;
        const std::string hostName = SystemUtils::GetHostName();

        // General
        Configuration::instance().SetValue<std::string>("awsmock.region", "eu-central-1");
        Configuration::instance().SetValue<std::string>("awsmock.user", "none");
        Configuration::instance().SetValue<std::string>("awsmock.access.key-id", "none");
        Configuration::instance().SetValue<std::string>("awsmock.access.account-id", "000000000000");
        Configuration::instance().SetValue<std::string>("awsmock.access.client-id", "00000000");
        Configuration::instance().SetValue<std::string>("awsmock.access.secret-access-key", "none");
        Configuration::instance().SetValue<std::string>("awsmock.temp-dir", "/tmp/awsmock/data/tmp");
        Configuration::instance().SetValue<std::string>("awsmock.data-dir", "/tmp/awsmock/data");
        Configuration::instance().SetValue<std::string>("awsmock.magic-file", DEFAULT_MAGIC_FILE);

        // Gateway
        Configuration::instance().SetValue<bool>("awsmock.gateway.active", true);
        Configuration::instance().SetValue<std::string>("awsmock.gateway.http.host", hostName);
        Configuration::instance().SetValue<int>("awsmock.gateway.http.port", port);
        Configuration::instance().SetValue<std::string>("awsmock.gateway.http.address", "0.0.0.0");
        Configuration::instance().SetValue<int>("awsmock.gateway.http.max-queue", 10);
        Configuration::instance().SetValue<int>("awsmock.gateway.http.max-body", 104857600);
        Configuration::instance().SetValue<int>("awsmock.gateway.http.timeout", 900);

        // Mongo DB
        Configuration::instance().SetValue<bool>("awsmock.mongodb.active", withDatabase);
        Configuration::instance().SetValue<std::string>("awsmock.mongodb.name", "test");
        Configuration::instance().SetValue<std::string>("awsmock.mongodb.host", hostName);
        Configuration::instance().SetValue<int>("awsmock.mongodb.port", 27017);
        Configuration::instance().SetValue<std::string>("awsmock.mongodb.user", "root");
        Configuration::instance().SetValue<std::string>("awsmock.mongodb.password", "password");
        Configuration::instance().SetValue<int>("awsmock.mongodb.pool-size", 64);

        // S3 configuration
        Configuration::instance().SetValue<bool>("awsmock.modules.s3.active", true);
        Configuration::instance().SetValue<std::string>("awsmock.modules.s3.data-dir", "/tmp/awsmock/data/s3");

        // SQS configuration
        Configuration::instance().SetValue<bool>("awsmock.modules.sqs.active", true);
        Configuration::instance().SetValue<int>("awsmock.modules.sqs.receive-poll", 1000);

        // SNS configuration
        Configuration::instance().SetValue<bool>("awsmock.modules.sns.active", true);

        // Lambda configuration
        Configuration::instance().SetValue<bool>("awsmock.modules.lambda.active", true);
        Configuration::instance().SetValue<std::string>("awsmock.modules.lambda.runtime.java11", "public.ecr.aws/lambda/java:11");

        // Transfer configuration
        Configuration::instance().SetValue<bool>("awsmock.modules.transfer.active", true);

        // Cognito configuration
        Configuration::instance().SetValue<bool>("awsmock.modules.cognito.active", true);

        // DynamoDB configuration
        Configuration::instance().SetValue<bool>("awsmock.modules.dynamodb.active", true);
        Configuration::instance().SetValue<std::string>("awsmock.modules.dynamodb.container.host", "localhost");
        Configuration::instance().SetValue<int>("awsmock.modules.dynamodb.container.port", 8000);

        // Docker
        Configuration::instance().SetValue<bool>("awsmock.docker.active", true);
        Configuration::instance().SetValue<std::string>("awsmock.docker.network-mode", "bridge");
        Configuration::instance().SetValue<std::string>("awsmock.docker.network-name", "local");
        Configuration::instance().SetValue<std::string>("awsmock.docker.socket", "/var/run/docker.sock");

        // Logging
        Configuration::instance().SetValue<std::string>("awsmock.logging.level", "error");
        Configuration::instance().SetValue<std::string>("awsmock.logging.prefix", "/tmp/awsmock-test.log");
        Configuration::instance().SetValue<long>("awsmock.logging.file-size", 1024 * 1024);
        Configuration::instance().SetValue<int>("awsmock.logging.file-count", 5);

        // Monitoring
        Configuration::instance().SetValue<int>("awsmock.monitoring.port", 19091);

        // Podman
        Configuration::instance().SetValue<bool>("awsmock.podman.active", false);
        Configuration::instance().SetValue<std::string>("awsmock.podman.network-mode", "local");
        Configuration::instance().SetValue<std::string>("awsmock.podman.network-name", "local");
        Configuration::instance().SetValue<std::string>("awsmock.podman.socket", "/var/run/podman/podman.sock");

        // Write a file
        Configuration::instance().WriteFile(TMP_CONFIGURATION_FILE);
    }

    std::string TestUtils::GetTestConfigurationFilename() {
        return TMP_CONFIGURATION_FILE;
    }

    Configuration &TestUtils::GetTestConfiguration(const bool withDatabase) {
        CreateTestConfigurationFile(withDatabase);
        Configuration &configuration = Configuration::instance();
        configuration.SetFilename(GetTestConfigurationFilename());
        return configuration;
    }

    std::string TestUtils::SendCliCommand(const std::string &command, const std::vector<std::string> &args) {
        std::string output, error;
        SystemUtils::RunShellCommand(command, args, output, error);
        if (!error.empty()) {
            std::cerr << error << std::endl;
        }
        return output;
    }

}// namespace AwsMock::Core
