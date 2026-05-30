//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SECRETSMANAGER_DATABASE_TEST_H
#define AWMOCK_CORE_SECRETSMANAGER_DATABASE_TEST_H

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SecretsManagerDatabase.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#ifndef SECRET_TEST_CONSTANTS
#define SECRET_TEST_CONSTANTS
#define REGION "eu-central-1"
#define SECRET_NAME "test-secret"
#endif

namespace AwsMock::Database {

    struct SecretsManagerDatabaseTest {

        SecretsManagerDatabaseTest() {
            _region = _configuration.get<std::string>("awsmock.region");
        }

        ~SecretsManagerDatabaseTest() {
            const long count = _secretsManagerDatabase.DeleteAllSecrets();
            log_debug << "Secrets deleted, count: " << count;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(true);
        SecretsManagerDatabase _secretsManagerDatabase = SecretsManagerDatabase();
    };

    BOOST_FIXTURE_TEST_CASE(SecretCreateTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.CreateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(result.secretId.empty(), false);
        BOOST_CHECK_EQUAL(result.arn.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SecretGetBySecretIdTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.GetSecretBySecretId(secret.secretId);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SecretGetByRegionNameTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.GetSecretByRegionName(secret.region, secret.name);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SecretGetByOidTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.GetSecretById(secret.oid);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SecretGetByArnTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);
        BOOST_REQUIRE_EQUAL(secret.arn.empty(), false);

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.GetSecretByArn(secret.arn);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.arn, secret.arn);
    }

    BOOST_FIXTURE_TEST_CASE(SecretExistsBySecretIdTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const bool result = _secretsManagerDatabase.SecretExists(secret.secretId);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(SecretExistsByRegionNameTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const bool result = _secretsManagerDatabase.SecretExists(secret.region, secret.name);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(SecretExistsByEntityTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const bool result = _secretsManagerDatabase.SecretExists(secret);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(SecretExistsByArnTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);
        BOOST_REQUIRE_EQUAL(secret.arn.empty(), false);

        // act
        const bool result = _secretsManagerDatabase.SecretExistsByArn(secret.arn);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(SecretNotExistsTest, SecretsManagerDatabaseTest) {

        // act
        const bool result = _secretsManagerDatabase.SecretExists("nonexistent-secret-id");

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(SecretUpdateTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        secret.description += " updated";
        secret = _secretsManagerDatabase.UpdateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(secret.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(secret.region, REGION);
        BOOST_CHECK_EQUAL(secret.description, "Test secret updated");
        BOOST_CHECK_EQUAL(secret.oid.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SecretCreateOrUpdateCreateTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.CreateOrUpdateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(result.name, SECRET_NAME);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(_secretsManagerDatabase.CountSecrets(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(SecretCreateOrUpdateUpdateTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        secret.description = "Updated description";
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.CreateOrUpdateSecret(secret);

        // assert
        BOOST_CHECK_EQUAL(result.description, "Updated description");
        BOOST_CHECK_EQUAL(_secretsManagerDatabase.CountSecrets(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(SecretCountTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret1;
        secret1.region = _region;
        secret1.name = SECRET_NAME;
        secret1.description = "Test secret";
        secret1 = _secretsManagerDatabase.CreateSecret(secret1);
        Entity::SecretsManager::Secret secret2;
        secret2.region = _region;
        secret2.name = std::string(SECRET_NAME) + "2";
        secret2.description = "Test secret 2";
        secret2 = _secretsManagerDatabase.CreateSecret(secret2);

        // act
        const long total = _secretsManagerDatabase.CountSecrets();
        const long byRegion = _secretsManagerDatabase.CountSecrets(_region);

        // assert
        BOOST_CHECK_EQUAL(total, 2);
        BOOST_CHECK_EQUAL(byRegion, 2);
    }

    BOOST_FIXTURE_TEST_CASE(SecretListTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::SecretList secretList = _secretsManagerDatabase.ListSecrets();

        // assert
        BOOST_CHECK_EQUAL(secretList.empty(), false);
        BOOST_CHECK_EQUAL(secretList.size(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(SecretDeleteTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret;
        secret.region = _region;
        secret.name = SECRET_NAME;
        secret.description = "Test secret";
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        _secretsManagerDatabase.DeleteSecret(secret);
        const Entity::SecretsManager::SecretList secretList = _secretsManagerDatabase.ListSecrets();

        // assert
        BOOST_CHECK_EQUAL(secretList.empty(), true);
    }

    BOOST_FIXTURE_TEST_CASE(SecretDeleteAllTest, SecretsManagerDatabaseTest) {

        // arrange
        Entity::SecretsManager::Secret secret1;
        secret1.region = _region;
        secret1.name = SECRET_NAME;
        secret1.description = "Test secret";
        secret1 = _secretsManagerDatabase.CreateSecret(secret1);
        Entity::SecretsManager::Secret secret2;
        secret2.region = _region;
        secret2.name = std::string(SECRET_NAME) + "2";
        secret2.description = "Test secret 2";
        secret2 = _secretsManagerDatabase.CreateSecret(secret2);

        // act
        const long count = _secretsManagerDatabase.DeleteAllSecrets();
        const Entity::SecretsManager::SecretList secretList = _secretsManagerDatabase.ListSecrets();

        // assert
        BOOST_CHECK_EQUAL(count, 2);
        BOOST_CHECK_EQUAL(secretList.empty(), true);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_SECRETSMANAGER_DATABASE_TEST_H
