//
// Created by vogje01 on 7/7/26.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/secretsmanager/SecretsManagerService.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_SECRET_NAME "test-secret"
#define TEST_SECRET_STRING R"({"username":"admin","password":"s3cr3t"})"
#define TEST_SECRET_STRING_V2 R"({"username":"admin","password":"n3wPassw0rd"})"
#define TEST_DESCRIPTION "test secret description"

namespace Awsmock::Database {

    Dto::SecretsManager::CreateSecretResponse CreateDefaultSecret(const Service::SecretsManagerService &svc) {
        Dto::SecretsManager::CreateSecretRequest request;
        request.region = TEST_REGION;
        request.name = TEST_SECRET_NAME;
        request.secretString = TEST_SECRET_STRING;
        request.description = TEST_DESCRIPTION;
        return svc.CreateSecret(request);
    }

    struct SecretsManagerServiceFixture {
        SecretsManagerServiceFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB, "test");
        }
        ~SecretsManagerServiceFixture() {
            try {
                const long count = RepositoryFactory::instance().secretsmanagerRepository()->DeleteAllSecrets();
                log_debug << "Secrets deleted, count: " << count;
            } catch (const std::exception &exc) {
                log_error << "SecretsManager fixture cleanup failed: " << exc.what();
            }
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SecretsManagerServiceTests, SecretsManagerServiceFixture)

    BOOST_AUTO_TEST_CASE(CreateSecretTest) {

        // arrange
        const Service::SecretsManagerService svc;

        // act
        const auto response = CreateDefaultSecret(svc);

        // assert
        BOOST_CHECK_EQUAL(false, response.arn.empty());
        BOOST_CHECK_EQUAL(false, response.versionId.empty());
        BOOST_CHECK_EQUAL(TEST_SECRET_NAME, response.name);
    }

    BOOST_AUTO_TEST_CASE(DescribeSecretTest) {

        // arrange
        const Service::SecretsManagerService svc;
        const auto created = CreateDefaultSecret(svc);
        BOOST_CHECK_EQUAL(false, created.arn.empty());
        // secretId in DB is "name-<hex>"; extract it from the ARN's last segment
        const std::string secretId = created.arn.substr(created.arn.rfind(':') + 1);

        Dto::SecretsManager::DescribeSecretRequest request;
        request.region = TEST_REGION;
        request.secretId = secretId;

        // act
        const auto response = svc.DescribeSecret(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_SECRET_NAME, response.name);
        BOOST_CHECK_EQUAL(false, response.arn.empty());
        BOOST_CHECK_EQUAL(TEST_DESCRIPTION, response.description);
    }

    BOOST_AUTO_TEST_CASE(GetSecretValueTest) {

        // arrange
        const Service::SecretsManagerService svc;
        const auto created = CreateDefaultSecret(svc);
        BOOST_CHECK_EQUAL(false, created.arn.empty());

        Dto::SecretsManager::GetSecretValueRequest request;
        request.region = TEST_REGION;
        request.secretId = created.arn;

        // act
        const auto response = svc.GetSecretValue(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_SECRET_NAME, response.name);
        BOOST_CHECK_EQUAL(false, response.arn.empty());
        BOOST_CHECK_EQUAL(TEST_SECRET_STRING, response.secretString);
        BOOST_CHECK_EQUAL(false, response.versionId.empty());
    }

    BOOST_AUTO_TEST_CASE(ListSecretsTest) {

        // arrange
        const Service::SecretsManagerService svc;
        Dto::SecretsManager::CreateSecretRequest req1;
        req1.region = TEST_REGION;
        req1.name = "test-secret-1";
        req1.secretString = TEST_SECRET_STRING;
        std::ignore = svc.CreateSecret(req1);
        Dto::SecretsManager::CreateSecretRequest req2;
        req2.region = TEST_REGION;
        req2.name = "test-secret-2";
        req2.secretString = TEST_SECRET_STRING;
        std::ignore = svc.CreateSecret(req2);

        Dto::SecretsManager::ListSecretsRequest listRequest;
        listRequest.region = TEST_REGION;

        // act
        const auto response = svc.ListSecrets(listRequest);

        // assert
        BOOST_CHECK_EQUAL(2, response.secretList.size());
    }

    BOOST_AUTO_TEST_CASE(PutSecretValueTest) {

        // arrange
        const Service::SecretsManagerService svc;
        const auto created = CreateDefaultSecret(svc);
        BOOST_CHECK_EQUAL(false, created.arn.empty());

        Dto::SecretsManager::PutSecretValueRequest putRequest;
        putRequest.region = TEST_REGION;
        putRequest.secretId = created.arn;
        putRequest.secretString = TEST_SECRET_STRING_V2;

        // act
        const auto putResponse = svc.PutSecretValue(putRequest);

        // assert
        BOOST_CHECK_EQUAL(TEST_SECRET_NAME, putResponse.name);
        BOOST_CHECK_EQUAL(false, putResponse.versionId.empty());

        // verify the new value is returned
        Dto::SecretsManager::GetSecretValueRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.secretId = created.arn;
        const auto getResponse = svc.GetSecretValue(getRequest);
        BOOST_CHECK_EQUAL(TEST_SECRET_STRING_V2, getResponse.secretString);
    }

    BOOST_AUTO_TEST_CASE(ListSecretVersionIdsTest) {

        // arrange
        const Service::SecretsManagerService svc;
        const auto created = CreateDefaultSecret(svc);
        BOOST_CHECK_EQUAL(false, created.arn.empty());

        // put a second version
        Dto::SecretsManager::PutSecretValueRequest putRequest;
        putRequest.region = TEST_REGION;
        putRequest.secretId = created.arn;
        putRequest.secretString = TEST_SECRET_STRING_V2;
        std::ignore = svc.PutSecretValue(putRequest);

        Dto::SecretsManager::ListSecretVersionIdsRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.secretId = created.arn.substr(created.arn.rfind(':') + 1);

        // act
        const auto response = svc.ListSecretVersionIds(listRequest);

        // assert
        BOOST_CHECK_EQUAL(TEST_SECRET_NAME, response.name);
        BOOST_CHECK_EQUAL(false, response.arn.empty());
        BOOST_CHECK_EQUAL(false, response.versions.empty());
    }

    BOOST_AUTO_TEST_CASE(UpdateSecretTest) {

        // arrange
        const Service::SecretsManagerService svc;
        const auto created = CreateDefaultSecret(svc);
        BOOST_CHECK_EQUAL(false, created.arn.empty());

        Dto::SecretsManager::UpdateSecretRequest request;
        request.region = TEST_REGION;
        request.secretId = created.arn.substr(created.arn.rfind(':') + 1);
        request.secretString = TEST_SECRET_STRING_V2;
        request.description = "updated description";

        // act
        const auto response = svc.UpdateSecret(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_SECRET_NAME, response.name);
        BOOST_CHECK_EQUAL(false, response.arn.empty());
        BOOST_CHECK_EQUAL(false, response.versionId.empty());

        // verify description updated
        Dto::SecretsManager::DescribeSecretRequest descRequest;
        descRequest.region = TEST_REGION;
        descRequest.secretId = created.arn.substr(created.arn.rfind(':') + 1);
        const auto descResponse = svc.DescribeSecret(descRequest);
        BOOST_CHECK_EQUAL("updated description", descResponse.description);
    }

    BOOST_AUTO_TEST_CASE(DeleteSecretTest) {

        // arrange
        const Service::SecretsManagerService svc;
        const auto created = CreateDefaultSecret(svc);
        BOOST_CHECK_EQUAL(false, created.arn.empty());

        Dto::SecretsManager::DeleteSecretRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.secretId = created.arn;
        deleteRequest.forceDeleteWithoutRecovery = true;

        // act
        const auto response = svc.DeleteSecret(deleteRequest);

        // assert
        BOOST_CHECK_EQUAL(TEST_SECRET_NAME, response.name);
        BOOST_CHECK_EQUAL(false, response.arn.empty());

        // verify it's gone
        Dto::SecretsManager::ListSecretsRequest listRequest;
        listRequest.region = TEST_REGION;
        const auto listResponse = svc.ListSecrets(listRequest);
        BOOST_CHECK_EQUAL(0, listResponse.secretList.size());
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
