//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_SERVICE_KMS_SERVICE_TEST_H
#define AWMOCK_SERVICE_KMS_SERVICE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/kms/KMSService.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define PLAIN_TEXT "The quick brown fox jumps over the lazy dog"
#define SYMMETRIC_BASE64_PLAIN_TEXT Core::Crypto::Base64Encode(PLAIN_TEXT)

namespace AwsMock::Service {

    class KMSServiceTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        void TearDown() override {
            _kmsDatabase.DeleteAllKeys();
        }

        void WaitForKey(const Dto::KMS::Key &key) const {

            Database::Entity::KMS::Key keyEntity;
            do {
                keyEntity = _kmsDatabase.GetKeyByKeyId(key.keyId);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } while (keyEntity.keyState != KeyStateToString(Dto::KMS::KeyState::ENABLED));
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
        KMSService _kmsService;
    };

    TEST_F(KMSServiceTest, CreateSymmetricKeyTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        EXPECT_FALSE(response.key.keyId.empty());
        EXPECT_FALSE(response.key.enabled);
        EXPECT_TRUE(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE);
        EXPECT_EQ(1, keyCount);
    }

    TEST_F(KMSServiceTest, CreateRSA2048KeyTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        EXPECT_FALSE(response.key.keyId.empty());
        EXPECT_FALSE(response.key.enabled);
        EXPECT_TRUE(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE);
        EXPECT_EQ(1, keyCount);
    }

    TEST_F(KMSServiceTest, CreateRSA3072KeyTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_3072;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        EXPECT_FALSE(response.key.keyId.empty());
        EXPECT_FALSE(response.key.enabled);
        EXPECT_TRUE(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE);
        EXPECT_EQ(1, keyCount);
    }

    TEST_F(KMSServiceTest, CreateRSA4096KeyTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_4096;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        EXPECT_FALSE(response.key.keyId.empty());
        EXPECT_FALSE(response.key.enabled);
        EXPECT_TRUE(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE);
        EXPECT_EQ(1, keyCount);
    }

    TEST_F(KMSServiceTest, ListKeyTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        Dto::KMS::ListKeysRequest listRequest;
        listRequest.region = REGION;
        listRequest.limit = 10;

        // act
        Dto::KMS::ListKeysResponse listResponse = _kmsService.ListKeys(listRequest);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        EXPECT_FALSE(response.key.keyId.empty());
        EXPECT_FALSE(response.key.enabled);
        EXPECT_TRUE(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE);
        EXPECT_EQ(1, keyCount);
    }

    TEST_F(KMSServiceTest, SymmetricEncryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForAesKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        EXPECT_FALSE(encryptResponse.keyId.empty());
        EXPECT_FALSE(encryptResponse.ciphertext.empty());
    }

    TEST_F(KMSServiceTest, SymmetricDecryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForAesKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = _region;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        EXPECT_FALSE(decryptResponse.keyId.empty());
        EXPECT_TRUE(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT);
    }

    TEST_F(KMSServiceTest, RSA2048EncryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        EXPECT_FALSE(encryptResponse.keyId.empty());
        EXPECT_FALSE(encryptResponse.ciphertext.empty());
    }

    TEST_F(KMSServiceTest, RSA2024DecryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);

        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = _region;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        EXPECT_FALSE(decryptResponse.keyId.empty());
        EXPECT_TRUE(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT);
    }

    TEST_F(KMSServiceTest, RSA3072EncryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_3072;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        EXPECT_FALSE(encryptResponse.keyId.empty());
        EXPECT_FALSE(encryptResponse.ciphertext.empty());
    }

    TEST_F(KMSServiceTest, RSA3072DecryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_3072;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = _region;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        EXPECT_FALSE(decryptResponse.keyId.empty());
        EXPECT_TRUE(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT);
    }

    TEST_F(KMSServiceTest, RSA4096EncryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_4096;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        EXPECT_FALSE(encryptResponse.keyId.empty());
        EXPECT_FALSE(encryptResponse.ciphertext.empty());
    }

    TEST_F(KMSServiceTest, RSA4096DecryptTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_4096;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);

        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = _region;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = _region;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        EXPECT_FALSE(decryptResponse.keyId.empty());
        EXPECT_TRUE(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT);
    }

    TEST_F(KMSServiceTest, DeleteKeyTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = _region;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        long keyCount = _kmsDatabase.CountKeys();
        std::string keyId = response.key.keyId;
        EXPECT_EQ(1, keyCount);

        Dto::KMS::ScheduleKeyDeletionRequest deleteRequest;
        deleteRequest.keyId = keyId;
        deleteRequest.pendingWindowInDays = 1;

        // act
        Dto::KMS::ScheduledKeyDeletionResponse deleteResponse = _kmsService.ScheduleKeyDeletion(deleteRequest);

        // assert
        EXPECT_TRUE(deleteResponse.keyState == Dto::KMS::KeyStateToString(Dto::KMS::KeyState::PENDING_DELETION));
        EXPECT_EQ(1, keyCount);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_LAMBDASERVICETEST_H