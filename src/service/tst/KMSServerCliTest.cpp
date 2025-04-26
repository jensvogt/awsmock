//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_KMS_SERVER_CLI_TEST_H
#define AWMOCK_KMS_SERVER_CLI_TEST_H

// GTest includes
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/cognito/CognitoServer.h>
#include <awsmock/service/cognito/CognitoService.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define PLAIN_TEXT_BASE64 std::string("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==")

namespace AwsMock::Service {

    /**
     * @brief AwsMock cognito integration test.
     */
    class KMSServerCliTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // Start gateway server
            StartGateway();

            // General configuration
            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {
            _database.DeleteAllKeys();
        }

        boost::thread _thread;
        std::string _endpoint, _region;
        boost::asio::io_context _ios{10};
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::KMSDatabase &_database = Database::KMSDatabase::instance();
    };

    TEST_F(KMSServerCliTest, KeyCreateSymmetricTest) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "SYMMETRIC_DEFAULT", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        EXPECT_EQ(1, keyList.size());
        EXPECT_FALSE(keyList.at(0).arn.empty());
        EXPECT_TRUE(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::SYMMETRIC_DEFAULT));
        EXPECT_TRUE(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT));
    }

    TEST_F(KMSServerCliTest, KeyCreateRSA2048Test) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_2048", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        EXPECT_EQ(1, keyList.size());
        EXPECT_FALSE(keyList.at(0).arn.empty());
        EXPECT_TRUE(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::RSA_2048));
        EXPECT_TRUE(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT));
    }

    TEST_F(KMSServerCliTest, KeyCreateRSA3072Test) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_3072", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        EXPECT_EQ(1, keyList.size());
        EXPECT_FALSE(keyList.at(0).arn.empty());
        EXPECT_TRUE(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::RSA_3072));
        EXPECT_TRUE(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT));
    }

    TEST_F(KMSServerCliTest, KeyCreateRSA4096Test) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_4096", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        EXPECT_EQ(1, keyList.size());
        EXPECT_FALSE(keyList.at(0).arn.empty());
        EXPECT_TRUE(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::RSA_4096));
        EXPECT_TRUE(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT));
    }

    TEST_F(KMSServerCliTest, KeyListTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_4096", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "list-keys", "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output2.empty());
        ASSERT_THAT(output2, testing::HasSubstr("Keys"));
        ASSERT_THAT(output2, testing::HasSubstr("KeyId"));
        ASSERT_THAT(output2, testing::HasSubstr("KeyArn"));
    }

    TEST_F(KMSServerCliTest, EncryptTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "SYMMETRIC_DEFAULT", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();
        const std::string keyId = keyList.at(0).keyId;

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "encrypt", "--key-id", keyId, "--plaintext", PLAIN_TEXT_BASE64, "--endpoint", _endpoint});
        Dto::KMS::EncryptResponse response;
        response.FromJson(output2);

        // assert
        EXPECT_FALSE(output2.empty());
        EXPECT_FALSE(response.ciphertext.empty());
    }

    TEST_F(KMSServerCliTest, DecryptTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "SYMMETRIC_DEFAULT", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();
        const std::string keyId = keyList.at(0).keyId;
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "encrypt", "--key-id", keyId, "--plaintext", PLAIN_TEXT_BASE64, "--endpoint", _endpoint});
        Dto::KMS::EncryptResponse response1;
        response1.FromJson(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "decrypt", "--key-id", keyId, "--ciphertext-blob", response1.ciphertext, "--endpoint", _endpoint});
        Dto::KMS::DecryptResponse response2;
        response2.FromJson(output3);

        // assert
        EXPECT_FALSE(output3.empty());
        EXPECT_TRUE(response2.plaintext == PLAIN_TEXT_BASE64);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_KMS_SERVER_CLI_TEST_H
