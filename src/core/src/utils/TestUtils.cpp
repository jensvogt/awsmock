//
// Created by vogje01 on 23/07/2023.
//

#include <awsmock/core/TestUtils.h>

namespace AwsMock::Core {

    void TestUtils::CreateTestConfigurationFile(const bool withDatabase) {

        // Logging
        if (getenv("AWSMOCK_TEST_LOG") != nullptr) {
            LogStream::Initialize();
            LogStream::SetFilename("/tmp/awsmock-test.log");
            LogStream::SetSeverity("debug");
        } else {
            LogStream::Initialize();
            LogStream::SetSeverity("none");
        }

        constexpr int port = 14566;
        const std::string hostName = SystemUtils::GetHostName();

        // General
        Configuration::instance().SetValueString("awsmock.region", "eu-central-1");
        Configuration::instance().SetValueString("awsmock.user", "none");
        Configuration::instance().SetValueString("awsmock.access.key-id", "none");
        Configuration::instance().SetValueString("awsmock.access.account-id", "000000000000");
        Configuration::instance().SetValueString("awsmock.access.client-id", "00000000");
        Configuration::instance().SetValueString("awsmock.access.secret-access-key", "none");
        Configuration::instance().SetValueString("awsmock.temp-dir", "/tmp/awsmock/data/tmp");
        Configuration::instance().SetValueString("awsmock.data-dir", "/tmp/awsmock/data");

        // Gateway
        Configuration::instance().SetValueBool("awsmock.gateway.active", true);
        Configuration::instance().SetValueString("awsmock.gateway.http.host", hostName);
        Configuration::instance().SetValueInt("awsmock.gateway.http.port", port);
        Configuration::instance().SetValueString("awsmock.gateway.http.address", "0.0.0.0");
        Configuration::instance().SetValueInt("awsmock.gateway.http.max-queue", 10);
        Configuration::instance().SetValueInt("awsmock.gateway.http.max-body", 104857600);
        Configuration::instance().SetValueInt("awsmock.gateway.http.timeout", 900);

        // Mongo DB
        Configuration::instance().SetValueBool("awsmock.mongodb.active", withDatabase);
        Configuration::instance().SetValueString("awsmock.mongodb.name", "test");
        Configuration::instance().SetValueString("awsmock.mongodb.host", hostName);
        Configuration::instance().SetValueInt("awsmock.mongodb.port", 27017);
        Configuration::instance().SetValueString("awsmock.mongodb.user", "root");
        Configuration::instance().SetValueString("awsmock.mongodb.password", "password");
        Configuration::instance().SetValueInt("awsmock.mongodb.pool-size", 64);

        // S3 configuration
        Configuration::instance().SetValueBool("awsmock.modules.s3.active", true);
        Configuration::instance().SetValueString("awsmock.modules.s3.data-dir", "/tmp/awsmock/data/s3");

        // SQS configuration
        Configuration::instance().SetValueBool("awsmock.modules.sqs.active", true);

        // SNS configuration
        Configuration::instance().SetValueBool("awsmock.modules.sns.active", true);

        // Lambda configuration
        Configuration::instance().SetValueBool("awsmock.modules.lambda.active", true);

        // Transfer configuration
        Configuration::instance().SetValueBool("awsmock.modules.transfer.active", true);

        // Cognito configuration
        Configuration::instance().SetValueBool("awsmock.modules.cognito.active", true);

        // DynamoDB configuration
        Configuration::instance().SetValueBool("awsmock.modules.dynamodb.active", true);
        Configuration::instance().SetValueString("awsmock.modules.dynamodb.container.host", "localhost");
        Configuration::instance().SetValueInt("awsmock.modules.dynamodb.container.port", 8000);

        // Docker
        Configuration::instance().SetValueString("awsmock.docker.network-mode", "bridge");
        Configuration::instance().SetValueString("awsmock.docker.network-name", "default");

        // Logging
        Configuration::instance().SetValueString("awsmock.logging.level", "debug");
        Configuration::instance().SetValueString("awsmock.logging.file", "/tmp/awsmock-test.log");

        // Write file
        Configuration::instance().WriteFile(TMP_PROPERTIES_FILE);
    }

    std::string TestUtils::GetTestConfigurationFilename() {
        return TMP_PROPERTIES_FILE;
    }

    Configuration &TestUtils::GetTestConfiguration(bool withDatabase) {
        CreateTestConfigurationFile(withDatabase);
        Configuration &configuration = Configuration::instance();
        configuration.SetFilename(GetTestConfigurationFilename());
        return configuration;
    }

    ExecResult TestUtils::SendCliCommand(const std::string &command) {
        return SystemUtils::Exec(command);
    }
}// namespace AwsMock::Core
