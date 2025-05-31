//
// Created by vogje01 on 02/06/2023.
//


// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/kms/KMSService.h>

#define BOOST_TEST_MODULE KMSServiceTests
#define REGION "eu-central-1"
#define OWNER "test-owner"
#define PLAIN_TEXT "The quick brown fox jumps over the lazy dog"
#define SYMMETRIC_BASE64_PLAIN_TEXT Core::Crypto::Base64Encode(PLAIN_TEXT)

namespace AwsMock::Service {

    struct KMSServiceTest : TestBase {

        KMSServiceTest() {
            // ReSharper disable once CppExpressionWithoutSideEffects
            _kmsDatabase.DeleteAllKeys();
        }

        void WaitForKey(const Dto::KMS::Key &key) const {

            Database::Entity::KMS::Key keyEntity;
            do {
                keyEntity = _kmsDatabase.GetKeyByKeyId(key.keyId);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } while (keyEntity.keyState != KeyStateToString(Dto::KMS::KeyState::ENABLED));
        }

        Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
        KMSService _kmsService;
    };

    BOOST_FIXTURE_TEST_CASE(CreateSymmetricKeyTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        BOOST_CHECK_EQUAL(response.key.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.key.enabled, false);
        BOOST_CHECK_EQUAL(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE, true);
        BOOST_CHECK_EQUAL(1, keyCount);
    }

    BOOST_FIXTURE_TEST_CASE(CreateRSA2048KeyTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        BOOST_CHECK_EQUAL(response.key.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.key.enabled, false);
        BOOST_CHECK_EQUAL(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE, true);
        BOOST_CHECK_EQUAL(1, keyCount);
    }

    BOOST_FIXTURE_TEST_CASE(CreateRSA3072KeyTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_3072;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        BOOST_CHECK_EQUAL(response.key.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.key.enabled, false);
        BOOST_CHECK_EQUAL(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE, true);
        BOOST_CHECK_EQUAL(1, keyCount);
    }

    BOOST_FIXTURE_TEST_CASE(CreateRSA4096KeyTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_4096;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        WaitForKey(response.key);
        long keyCount = _kmsDatabase.CountKeys();

        // assert
        BOOST_CHECK_EQUAL(response.key.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.key.enabled, false);
        BOOST_CHECK_EQUAL(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE, true);
        BOOST_CHECK_EQUAL(1, keyCount);
    }

    BOOST_FIXTURE_TEST_CASE(ListKeyTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
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
        BOOST_CHECK_EQUAL(response.key.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.key.enabled, false);
        BOOST_CHECK_EQUAL(response.key.keyState == Dto::KMS::KeyState::UNAVAILABLE, true);
        BOOST_CHECK_EQUAL(1, keyCount);
    }

    BOOST_FIXTURE_TEST_CASE(SymmetricEncryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForAesKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        BOOST_CHECK_EQUAL(encryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(encryptResponse.ciphertext.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SymmetricDecryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForAesKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = REGION;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        BOOST_CHECK_EQUAL(decryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT, true);
    }

    BOOST_FIXTURE_TEST_CASE(RSA2048EncryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        BOOST_CHECK_EQUAL(encryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(encryptResponse.ciphertext.empty(), false);
    }

    /*
    BOOST_FIXTURE_TEST_CASE(RSA2024DecryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);

        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::RSAES_OAEP_SHA_1;

        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = REGION;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::RSAES_OAEP_SHA_1;

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        std::string tmp = Core::Crypto::Base64Decode(decryptResponse.plaintext);
        BOOST_CHECK_EQUAL(decryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT, true);
    }

    BOOST_FIXTURE_TEST_CASE(RSA3072EncryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_3072;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::RSAES_OAEP_SHA_1;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        BOOST_CHECK_EQUAL(encryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(encryptResponse.ciphertext.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(RSA3072DecryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_3072;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::RSAES_OAEP_SHA_1;

        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = REGION;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::RSAES_OAEP_SHA_1;

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        BOOST_CHECK_EQUAL(decryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT, true);
    }

    BOOST_FIXTURE_TEST_CASE(RSA4096EncryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_4096;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);
        std::string keyId = response.key.keyId;

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);

        // assert
        BOOST_CHECK_EQUAL(encryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(encryptResponse.ciphertext.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(RSA4096DecryptTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::RSA_4096;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        _kmsService.WaitForRsaKey(response.key.keyId, 5);

        std::string keyId = response.key.keyId;
        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = REGION;
        encryptRequest.keyId = keyId;
        encryptRequest.plainText = SYMMETRIC_BASE64_PLAIN_TEXT;
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;
        Dto::KMS::EncryptResponse encryptResponse = _kmsService.Encrypt(encryptRequest);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = REGION;
        decryptRequest.keyId = keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act
        Dto::KMS::DecryptResponse decryptResponse = _kmsService.Decrypt(decryptRequest);

        // assert
        BOOST_CHECK_EQUAL(decryptResponse.keyId.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext.empty(), false);
        BOOST_CHECK_EQUAL(decryptResponse.plaintext == SYMMETRIC_BASE64_PLAIN_TEXT, true);
    }*/

    BOOST_FIXTURE_TEST_CASE(DeleteKeyTest, KMSServiceTest) {

        // arrange
        Dto::KMS::CreateKeyRequest request;
        request.requestId = Core::StringUtils::CreateRandomUuid();
        request.region = REGION;
        request.user = "user";
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        Dto::KMS::CreateKeyResponse response = _kmsService.CreateKey(request);
        long keyCount = _kmsDatabase.CountKeys();
        std::string keyId = response.key.keyId;
        BOOST_CHECK_EQUAL(1, keyCount);

        Dto::KMS::ScheduleKeyDeletionRequest deleteRequest;
        deleteRequest.keyId = keyId;
        deleteRequest.pendingWindowInDays = 1;

        // act
        Dto::KMS::ScheduledKeyDeletionResponse deleteResponse = _kmsService.ScheduleKeyDeletion(deleteRequest);

        // assert
        BOOST_CHECK_EQUAL(deleteResponse.keyState == Dto::KMS::KeyStateToString(Dto::KMS::KeyState::PENDING_DELETION), true);
        BOOST_CHECK_EQUAL(1, keyCount);
    }
}// namespace AwsMock::Service
