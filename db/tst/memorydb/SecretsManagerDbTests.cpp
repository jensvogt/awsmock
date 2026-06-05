//
// Created by vogje01 on 02/06/2023.
//

// Boost includes
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

// AwsMock includes
#include "awsmock/core/AwsUtils.h"


#include <../../include/awsmock/repository/secretsmanager/SecretsManagerDatabase.h>
#include <awsmock/core/TestUtils.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#ifndef SECRET_TEST_CONSTANTS
#define SECRET_TEST_CONSTANTS
#define REGION "eu-central-1"
#define SECRET_NAME "test-secret"
#endif

namespace Awsmock::Database {

    struct SecretsmanagerMemoryDbFixture {
        SecretsmanagerMemoryDbFixture() = default;
        ~SecretsmanagerMemoryDbFixture() {
            const long secretsCount = SecretsManagerDatabase::instance().DeleteAllSecrets();
            log_debug << "Secrets deleted " << secretsCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SecretsmanagerMemoryDbTests, SecretsmanagerMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(SecretCreateTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret.secretId = "test-secret-id";

        // act
        const Entity::SecretsManager::Secret result = secretsmanagerDatabase.CreateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(result.secretId.empty(), false);
    }

    BOOST_AUTO_TEST_CASE(SecretGetBySecretIdTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = secretsmanagerDatabase.GetSecretBySecretId(secret.secretId);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
    }

    BOOST_AUTO_TEST_CASE(SecretGetByRegionNameMTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = secretsmanagerDatabase.GetSecretByRegionName(secret.region, secret.name);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
    }

    BOOST_AUTO_TEST_CASE(SecretGetByOidTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = secretsmanagerDatabase.GetSecretById(secret.oid);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
    }

    BOOST_AUTO_TEST_CASE(SecretGetByArnTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret.arn = Core::AwsUtils::CreateSecretArn(REGION, "abc", "2763af");
        secret = secretsmanagerDatabase.CreateSecret(secret);
        BOOST_REQUIRE_EQUAL(secret.arn.empty(), false);

        // act
        const Entity::SecretsManager::Secret result = secretsmanagerDatabase.GetSecretByArn(secret.arn);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.arn, secret.arn);
    }

    BOOST_AUTO_TEST_CASE(SecretExistsBySecretIdTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        const bool result = secretsmanagerDatabase.SecretExists(secret.secretId);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(SecretExistsByRegionNameTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        const bool result = secretsmanagerDatabase.SecretExists(secret.region, secret.name);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(SecretExistsByEntityTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        const bool result = secretsmanagerDatabase.SecretExists(secret);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(SecretExistsByArnTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret.arn = Core::AwsUtils::CreateSecretArn(REGION, "abc", "2763af");
        secret = secretsmanagerDatabase.CreateSecret(secret);
        BOOST_REQUIRE_EQUAL(secret.arn.empty(), false);

        // act
        const bool result = secretsmanagerDatabase.SecretExistsByArn(secret.arn);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(SecretNotExistsTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();

        // act
        const bool result = secretsmanagerDatabase.SecretExists("nonexistent-secret-id");

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_CASE(SecretUpdateTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        secret.description += " updated";
        secret = secretsmanagerDatabase.UpdateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(secret.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(secret.region, REGION);
        BOOST_CHECK_EQUAL(secret.description, "Test secret updated");
        BOOST_CHECK_EQUAL(secret.oid.empty(), false);
    }

    BOOST_AUTO_TEST_CASE(SecretCreateOrUpdateCreateTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";

        // act
        const Entity::SecretsManager::Secret result = secretsmanagerDatabase.CreateOrUpdateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(secretsmanagerDatabase.CountSecrets(), 1);
    }

    BOOST_AUTO_TEST_CASE(SecretCreateOrUpdateUpdateTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        secret.description = "Updated description";
        const Entity::SecretsManager::Secret result = secretsmanagerDatabase.CreateOrUpdateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(result.description, "Updated description");
        BOOST_CHECK_EQUAL(secretsmanagerDatabase.CountSecrets(), 1);
    }

    BOOST_AUTO_TEST_CASE(SecretCountTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret1;
        secret1.region = REGION;
        secret1.name = SECRET_NAME;
        secret1.description = "Test secret";
        secret1 = secretsmanagerDatabase.CreateSecret(secret1);
        Entity::SecretsManager::Secret secret2;
        secret2.region = std::string(REGION) + "1";
        secret2.name = std::string(SECRET_NAME) + "2";
        secret2.description = "Test secret 2";
        secret2 = secretsmanagerDatabase.CreateSecret(secret2);

        // act
        const long total = secretsmanagerDatabase.CountSecrets();
        const long byRegion = secretsmanagerDatabase.CountSecrets(REGION);

        // assert
        BOOST_CHECK_EQUAL(total, 2);
        BOOST_CHECK_EQUAL(byRegion, 1);
        BOOST_CHECK_EQUAL(secret1.arn.empty(), true);
        BOOST_CHECK_EQUAL(secret2.arn.empty(), true);
    }

    BOOST_AUTO_TEST_CASE(SecretListTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::SecretList secretList = secretsmanagerDatabase.ListSecrets();

        // assert
        BOOST_CHECK_EQUAL(secretList.empty(), false);
        BOOST_CHECK_EQUAL(secretList.size(), 1);
        BOOST_CHECK_EQUAL(secret.arn.empty(), true);
    }

    BOOST_AUTO_TEST_CASE(SecretDeleteTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret;
        secret.region = REGION;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = secretsmanagerDatabase.CreateSecret(secret);

        // act
        secretsmanagerDatabase.DeleteSecret(secret);
        const Entity::SecretsManager::SecretList secretList = secretsmanagerDatabase.ListSecrets();

        // assert
        BOOST_CHECK_EQUAL(secretList.empty(), true);
    }

    BOOST_AUTO_TEST_CASE(SecretDeleteAllTest) {

        // arrange
        const SecretsManagerDatabase &secretsmanagerDatabase = SecretsManagerDatabase::instance();
        Entity::SecretsManager::Secret secret1;
        secret1.region = REGION;
        secret1.name = SECRET_NAME;
        secret1.description = "Test secret";
        secret1 = secretsmanagerDatabase.CreateSecret(secret1);
        Entity::SecretsManager::Secret secret2;
        secret2.region = REGION;
        secret2.name = std::string(SECRET_NAME) + "2";
        secret2.description = "Test secret 2";
        secret2 = secretsmanagerDatabase.CreateSecret(secret2);

        // act
        const long count = secretsmanagerDatabase.DeleteAllSecrets();
        const Entity::SecretsManager::SecretList secretList = secretsmanagerDatabase.ListSecrets();

        // assert
        BOOST_CHECK_EQUAL(count, 2);
        BOOST_CHECK_EQUAL(secretList.empty(), true);
        BOOST_CHECK_EQUAL(secret1.arn.empty(), true);
        BOOST_CHECK_EQUAL(secret2.arn.empty(), true);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
