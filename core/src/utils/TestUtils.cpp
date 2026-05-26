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
        Configuration::instance().set<std::string>("awsmock.region", "eu-central-1");
        Configuration::instance().set<std::string>("awsmock.user", "none");
        Configuration::instance().set<std::string>("awsmock.access.key-id", "none");
        Configuration::instance().set<std::string>("awsmock.access.account-id", "000000000000");
        Configuration::instance().set<std::string>("awsmock.access.client-id", "00000000");
        Configuration::instance().set<std::string>("awsmock.access.secret-access-key", "none");
        Configuration::instance().set<std::string>("awsmock.temp-dir", "/tmp/awsmock/data/tmp");
        Configuration::instance().set<std::string>("awsmock.data-dir", "/tmp/awsmock/data");
        Configuration::instance().set<std::string>("awsmock.magic-file", "/usr/local/awsmock/etc/magic_linux.mgc");

        // Gateway
        Configuration::instance().set<bool>("awsmock.gateway.active", true);
        Configuration::instance().set<std::string>("awsmock.gateway.http.host", hostName);
        Configuration::instance().set<int>("awsmock.gateway.http.port", port);
        Configuration::instance().set<std::string>("awsmock.gateway.http.address", "0.0.0.0");
        Configuration::instance().set<int>("awsmock.gateway.http.max-queue", 10);
        Configuration::instance().set<int>("awsmock.gateway.http.max-body", 104857600);
        Configuration::instance().set<int>("awsmock.gateway.http.timeout", 900);

        // Mongo DB
        Configuration::instance().set<bool>("awsmock.mongodb.active", withDatabase);
        Configuration::instance().set<std::string>("awsmock.mongodb.name", "test");
        Configuration::instance().set<std::string>("awsmock.mongodb.host", hostName);
        Configuration::instance().set<int>("awsmock.mongodb.port", 27017);
        Configuration::instance().set<std::string>("awsmock.mongodb.user", "root");
        Configuration::instance().set<std::string>("awsmock.mongodb.password", "password");
        Configuration::instance().set<int>("awsmock.mongodb.pool-size", 64);

        // S3 configuration
        Configuration::instance().set<bool>("awsmock.modules.s3.active", true);
        Configuration::instance().set<std::string>("awsmock.modules.s3.data-dir", "/tmp/awsmock/data/s3");

        // SQS configuration
        Configuration::instance().set<bool>("awsmock.modules.sqs.active", true);
        Configuration::instance().set<int>("awsmock.modules.sqs.receive-poll", 1000);

        // SNS configuration
        Configuration::instance().set<bool>("awsmock.modules.sns.active", true);

        // Lambda configuration
        Configuration::instance().set<bool>("awsmock.modules.lambda.active", true);
        Configuration::instance().set<std::string>("awsmock.modules.lambda.runtime.java11", "public.ecr.aws/lambda/java:11");

        // Transfer configuration
        Configuration::instance().set<bool>("awsmock.modules.transfer.active", true);

        // Cognito configuration
        Configuration::instance().set<bool>("awsmock.modules.cognito.active", true);

        // DynamoDB configuration
        Configuration::instance().set<bool>("awsmock.modules.dynamodb.active", true);
        Configuration::instance().set<std::string>("awsmock.modules.dynamodb.container.host", "localhost");
        Configuration::instance().set<int>("awsmock.modules.dynamodb.container.port", 8000);

        // Docker
        Configuration::instance().set<bool>("awsmock.docker.active", true);
        Configuration::instance().set<std::string>("awsmock.docker.network-mode", "bridge");
        Configuration::instance().set<std::string>("awsmock.docker.network-name", "local");
        Configuration::instance().set<std::string>("awsmock.docker.socket", "/var/run/docker.sock");

        // Logging
        Configuration::instance().set<std::string>("awsmock.logging.level", "error");
        Configuration::instance().set<std::string>("awsmock.logging.prefix", "/tmp/awsmock-test.log");
        Configuration::instance().set<long>("awsmock.logging.file-size", 1024 * 1024);
        Configuration::instance().set<int>("awsmock.logging.file-count", 5);

        // Monitoring
        Configuration::instance().set<int>("awsmock.monitoring.port", 19091);

        // Write a file
        Configuration::instance().setFilePath(TMP_CONFIGURATION_FILE);
        Configuration::instance().save();
    }

    std::string TestUtils::GetTestConfigurationFilename() {
        return TMP_CONFIGURATION_FILE;
    }

    Configuration &TestUtils::GetTestConfiguration(const bool withDatabase) {
        CreateTestConfigurationFile(withDatabase);
        Configuration &configuration = Configuration::instance();
        configuration.setFilePath(GetTestConfigurationFilename());
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

} // namespace AwsMock::Core
