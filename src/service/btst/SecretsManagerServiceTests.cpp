//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_SERVICE_SECRETS_MANAGER_SERVICE_TEST_H
#define AWMOCK_SERVICE_SECRETS_MANAGER_SERVICE_TEST_H

// Test includes
#include "TestBase.h"
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define PLAIN_TEXT "The quick brown fox jumps over the lazy dog"
#define SYMMETRIC_BASE64_PLAIN_TEXT Core::Crypto::Base64Encode(PLAIN_TEXT)

namespace AwsMock::Service {

    struct SecretsServiceTest : TestBase {

        SecretsServiceTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~SecretsServiceTest() {
            const long count = _secretsManagerDatabase.DeleteAllSecrets();
            log_debug << "Keys deleted: " << count;
        }

        [[nodiscard]] Dto::SecretsManager::CreateSecretRequest createDefaultRequest() const {
            Dto::SecretsManager::CreateSecretRequest request;
            request.requestId = Core::StringUtils::CreateRandomUuid();
            request.region = _region;
            request.user = "user";
            request.description = "description";
            request.name = "test-secret";
            request.secretString = "secret-string";
            return request;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        Database::SecretsManagerDatabase &_secretsManagerDatabase = Database::SecretsManagerDatabase::instance();
        SecretsManagerService _secretsManagerService;
    };

    BOOST_FIXTURE_TEST_CASE(CreateSecretTest, SecretsServiceTest) {

        // arrange
        const Dto::SecretsManager::CreateSecretRequest request = createDefaultRequest();

        // act
        const Dto::SecretsManager::CreateSecretResponse response = _secretsManagerService.CreateSecret(request);
        const long keyCount = _secretsManagerDatabase.CountSecrets();
        const Database::Entity::SecretsManager::Secret secret = _secretsManagerDatabase.GetSecretByArn(response.arn);

        // assert
        BOOST_CHECK_EQUAL(1, keyCount);
        BOOST_CHECK_EQUAL(response.name, "test-secret");
        BOOST_CHECK_EQUAL(response.versionId.empty(), false);
        BOOST_CHECK_EQUAL(secret.versions.empty(), false);
        BOOST_CHECK_EQUAL(secret.versions.begin()->second.secretString.empty(), false);
        BOOST_CHECK_EQUAL(*secret.versions.begin()->second.stages.begin() == Dto::SecretsManager::VersionStateToString(Dto::SecretsManager::VersionStateType::AWSCURRENT), true);
        BOOST_CHECK_EQUAL(secret.kmsKeyId.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(GetSecretValueTest, SecretsServiceTest) {

        // arrange
        const Dto::SecretsManager::CreateSecretRequest createRequest = createDefaultRequest();
        const Dto::SecretsManager::CreateSecretResponse createResponse = _secretsManagerService.CreateSecret(createRequest);

        // act
        Dto::SecretsManager::GetSecretValueRequest getRequest;
        getRequest.region = _region;
        getRequest.secretId = createResponse.arn;
        getRequest.versionStage = "AWSCURRENT";

        Dto::SecretsManager::GetSecretValueResponse getResponse = _secretsManagerService.GetSecretValue(getRequest);
        const long keyCount = _secretsManagerDatabase.CountSecrets();

        // assert
        BOOST_CHECK_EQUAL(1, keyCount);
        BOOST_CHECK_EQUAL(getResponse.name, "test-secret");
        BOOST_CHECK_EQUAL(getResponse.versionId.empty(), false);
        BOOST_CHECK_EQUAL(getResponse.secretString, createRequest.secretString);
    }
}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_SECRETS_MANAGER_SERVICE_TEST_H