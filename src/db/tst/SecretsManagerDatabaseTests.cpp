//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SECRETSMANAGER_DATABASE_TEST_H
#define AWMOCK_CORE_SECRETSMANAGER_DATABASE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SecretsManagerDatabase.h>

#define REGION "eu-central-1"
#define SECRET_NAME "test-secret"
#define OWNER "test-owner"

namespace AwsMock::Database {

    class SecretsManagerDatabaseTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.GetValueString("awsmock.region");
        }

        void TearDown() override {
            _secretsManagerDatabase.DeleteAllSecrets();
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        SecretsManagerDatabase &_secretsManagerDatabase = SecretsManagerDatabase::instance();
    };

    TEST_F(SecretsManagerDatabaseTest, SecretCreateTest) {

        // arrange
        const Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.CreateSecret(secret);

        // assert
        EXPECT_TRUE(result.name == SECRET_NAME);
        EXPECT_TRUE(result.region == REGION);
        EXPECT_FALSE(result.oid.empty());
    }

    TEST_F(SecretsManagerDatabaseTest, SecretGetByIdTest) {

        // arrange
        Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.GetSecretBySecretId(secret.secretId);

        // assert
        EXPECT_TRUE(result.name == SECRET_NAME);
        EXPECT_TRUE(result.region == REGION);
        EXPECT_FALSE(result.oid.empty());
    }

    TEST_F(SecretsManagerDatabaseTest, SecretGetByRegionNameTest) {

        // arrange
        Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.GetSecretByRegionName(secret.region, secret.name);

        // assert
        EXPECT_TRUE(result.name == SECRET_NAME);
        EXPECT_TRUE(result.region == REGION);
        EXPECT_FALSE(result.oid.empty());
    }

    TEST_F(SecretsManagerDatabaseTest, SecretGetByOidTest) {

        // arrange
        Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        const Entity::SecretsManager::Secret result = _secretsManagerDatabase.GetSecretById(secret.oid);

        // assert
        EXPECT_TRUE(result.name == SECRET_NAME);
        EXPECT_TRUE(result.region == REGION);
        EXPECT_FALSE(result.oid.empty());
    }

    TEST_F(SecretsManagerDatabaseTest, SecretExistsTest) {

        // arrange
        Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        bool result = _secretsManagerDatabase.SecretExists(secret.secretId);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(SecretsManagerDatabaseTest, SecretUpdateTest) {

        // arrange
        // arrange
        Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        secret.description += "update";
        secret = _secretsManagerDatabase.UpdateSecret(secret);

        // assert
        EXPECT_TRUE(secret.name == SECRET_NAME);
        EXPECT_TRUE(secret.region == REGION);
        EXPECT_TRUE(Core::StringUtils::EqualsIgnoreCase(secret.description, "Test secretupdate"));
        EXPECT_FALSE(secret.oid.empty());
    }

    TEST_F(SecretsManagerDatabaseTest, SecretListTest) {

        // arrange
        // arrange
        Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        Entity::SecretsManager::SecretList secretList = _secretsManagerDatabase.ListSecrets();

        // assert
        EXPECT_FALSE(secretList.empty());
        EXPECT_EQ(1, secretList.size());
    }

    TEST_F(SecretsManagerDatabaseTest, SecretDeleteTest) {

        // arrange
        Entity::SecretsManager::Secret secret = {.region = _region, .name = SECRET_NAME, .description = "Test secret"};
        secret = _secretsManagerDatabase.CreateSecret(secret);

        // act
        _secretsManagerDatabase.DeleteSecret(secret);
        Entity::SecretsManager::SecretList secretList = _secretsManagerDatabase.ListSecrets();

        // assert
        EXPECT_TRUE(secretList.empty());
    }

    TEST_F(SecretsManagerDatabaseTest, SecretDeleteAllTest) {

        // arrange
        Entity::SecretsManager::Secret secret1 = {.region = _region, .name = SECRET_NAME, .description = "Test secret1"};
        secret1 = _secretsManagerDatabase.CreateSecret(secret1);
        Entity::SecretsManager::Secret secret2 = {.region = _region, .name = std::string(SECRET_NAME) + "1", .description = "Test secret2"};
        secret2 = _secretsManagerDatabase.CreateSecret(secret2);

        // act
        _secretsManagerDatabase.DeleteAllSecrets();
        Entity::SecretsManager::SecretList secretList = _secretsManagerDatabase.ListSecrets();

        // assert
        EXPECT_TRUE(secretList.empty());
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_SECRETSMANAGER_DATABASE_TEST_H