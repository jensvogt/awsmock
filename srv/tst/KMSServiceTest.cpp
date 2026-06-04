//
// Created by vogje01 on 30/05/2023.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/kms/IKMSRepository.h>
#include <awsmock/service/kms/KMSService.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_PLAINTEXT "Hello, KMS!"
#define TEST_DESCRIPTION "test-key-description"

namespace Awsmock::Database {

    Dto::KMS::CreateKeyResponse CreateDefaultSymmetricKey(const Service::KMSService &kmsService) {
        Dto::KMS::CreateKeyRequest request;
        request.region = TEST_REGION;
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        request.description = TEST_DESCRIPTION;
        return kmsService.CreateKey(request);
    }

    Dto::KMS::CreateKeyResponse CreateDefaultRsaKey(const Service::KMSService &kmsService) {
        Dto::KMS::CreateKeyRequest request;
        request.region = TEST_REGION;
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        return kmsService.CreateKey(request);
    }

    struct KMSServiceFixture {
        KMSServiceFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB);
        }
        ~KMSServiceFixture() {
            try {
                const long count = RepositoryFactory::instance().kmsRepository()->deleteAllKeys();
                log_debug << "KMS keys deleted, count: " << count;
            } catch (const std::exception &exc) {
                log_error << "KMS fixture cleanup failed: " << exc.what();
            }
        }
    };

    BOOST_FIXTURE_TEST_SUITE(KMSServiceTests, KMSServiceFixture)

    BOOST_AUTO_TEST_CASE(KeyCreateSymmetricTest) {

        // arrange
        const Service::KMSService kmsService;
        Dto::KMS::CreateKeyRequest request;
        request.region = TEST_REGION;
        request.keySpec = Dto::KMS::KeySpec::SYMMETRIC_DEFAULT;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;
        request.description = TEST_DESCRIPTION;

        // act
        const Dto::KMS::CreateKeyResponse response = kmsService.CreateKey(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.keyMetadata.keyId.empty());
        BOOST_CHECK_EQUAL(false, response.keyMetadata.arn.empty());
        BOOST_CHECK_EQUAL(Dto::KMS::KeySpec::SYMMETRIC_DEFAULT, response.keyMetadata.keySpec);
        BOOST_CHECK_EQUAL(TEST_DESCRIPTION, response.keyMetadata.description);
    }

    BOOST_AUTO_TEST_CASE(KeyCreateRsaTest) {

        // arrange
        const Service::KMSService kmsService;
        Dto::KMS::CreateKeyRequest request;
        request.region = TEST_REGION;
        request.keySpec = Dto::KMS::KeySpec::RSA_2048;
        request.keyUsage = Dto::KMS::KeyUsage::ENCRYPT_DECRYPT;

        // act
        const Dto::KMS::CreateKeyResponse response = kmsService.CreateKey(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.keyMetadata.keyId.empty());
        BOOST_CHECK_EQUAL(false, response.keyMetadata.arn.empty());
        BOOST_CHECK_EQUAL(Dto::KMS::KeySpec::RSA_2048, response.keyMetadata.keySpec);
    }

    BOOST_AUTO_TEST_CASE(KeyListTest) {

        // arrange
        const Service::KMSService kmsService;
        const Dto::KMS::CreateKeyResponse createResponse1 = CreateDefaultSymmetricKey(kmsService);
        BOOST_CHECK_EQUAL(false, createResponse1.keyMetadata.keyId.empty());

        const Dto::KMS::CreateKeyResponse createResponse2 = CreateDefaultSymmetricKey(kmsService);
        BOOST_CHECK_EQUAL(false, createResponse2.keyMetadata.keyId.empty());

        Dto::KMS::ListKeysRequest listRequest;
        listRequest.region = TEST_REGION;

        // act
        const Dto::KMS::ListKeysResponse response = kmsService.ListKeys(listRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.keys.empty());
        BOOST_CHECK_EQUAL(2, response.keys.size());
    }

    BOOST_AUTO_TEST_CASE(KeyDescribeTest) {

        // arrange
        const Service::KMSService kmsService;
        const Dto::KMS::CreateKeyResponse createResponse = CreateDefaultSymmetricKey(kmsService);
        BOOST_CHECK_EQUAL(false, createResponse.keyMetadata.keyId.empty());

        Dto::KMS::DescribeKeyRequest describeRequest;
        describeRequest.region = TEST_REGION;
        describeRequest.keyId = createResponse.keyMetadata.keyId;

        // act
        const Dto::KMS::DescribeKeyResponse response = kmsService.DescribeKey(describeRequest);

        // assert
        BOOST_CHECK_EQUAL(createResponse.keyMetadata.keyId, response.keyMetadata.keyId);
        BOOST_CHECK_EQUAL(createResponse.keyMetadata.arn, response.keyMetadata.arn);
        BOOST_CHECK_EQUAL(TEST_DESCRIPTION, response.keyMetadata.description);
    }

    BOOST_AUTO_TEST_CASE(KeyScheduleDeletionTest) {

        // arrange
        const Service::KMSService kmsService;
        const Dto::KMS::CreateKeyResponse createResponse = CreateDefaultSymmetricKey(kmsService);
        BOOST_CHECK_EQUAL(false, createResponse.keyMetadata.keyId.empty());

        Dto::KMS::ScheduleKeyDeletionRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.keyId = createResponse.keyMetadata.keyId;
        deleteRequest.pendingWindowInDays = 7;

        // act
        const Dto::KMS::ScheduledKeyDeletionResponse response = kmsService.ScheduleKeyDeletion(deleteRequest);

        // assert
        BOOST_CHECK_EQUAL(createResponse.keyMetadata.keyId, response.keyId);
        BOOST_CHECK_EQUAL(7, response.pendingWindowInDays);
        BOOST_CHECK_EQUAL(false, response.keyState.empty());
        BOOST_CHECK_GT(response.deletionDate, 0);
    }

    BOOST_AUTO_TEST_CASE(KeyDeleteTest) {

        // arrange
        const Service::KMSService kmsService;
        const Dto::KMS::CreateKeyResponse createResponse = CreateDefaultSymmetricKey(kmsService);
        BOOST_CHECK_EQUAL(false, createResponse.keyMetadata.keyId.empty());

        Dto::KMS::DeleteKeyRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.keyId = createResponse.keyMetadata.keyId;

        // act
        BOOST_CHECK_NO_THROW(kmsService.DeleteKey(deleteRequest));

        // assert
        Dto::KMS::ListKeysRequest listRequest;
        listRequest.region = TEST_REGION;
        const Dto::KMS::ListKeysResponse listResponse = kmsService.ListKeys(listRequest);
        BOOST_CHECK_EQUAL(true, listResponse.keys.empty());
    }

    BOOST_AUTO_TEST_CASE(SymmetricEncryptDecryptTest) {

        // arrange
        const Service::KMSService kmsService;
        const Dto::KMS::CreateKeyResponse createResponse = CreateDefaultSymmetricKey(kmsService);
        BOOST_CHECK_EQUAL(false, createResponse.keyMetadata.keyId.empty());

        // wait for async AES key material creation
        kmsService.WaitForAesKey(createResponse.keyMetadata.keyId, 10);

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = TEST_REGION;
        encryptRequest.keyId = createResponse.keyMetadata.keyId;
        encryptRequest.plaintext = Core::Crypto::Base64Encode(std::string(TEST_PLAINTEXT));
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        // act encrypt
        const Dto::KMS::EncryptResponse encryptResponse = kmsService.Encrypt(encryptRequest);

        // assert encrypt
        BOOST_CHECK_EQUAL(false, encryptResponse.ciphertext.empty());
        BOOST_CHECK_EQUAL(createResponse.keyMetadata.keyId, encryptResponse.keyId);

        // act decrypt
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = TEST_REGION;
        decryptRequest.keyId = createResponse.keyMetadata.keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::SYMMETRIC_DEFAULT;

        const Dto::KMS::DecryptResponse decryptResponse = kmsService.Decrypt(decryptRequest);

        // assert decrypt
        BOOST_CHECK_EQUAL(std::string(TEST_PLAINTEXT), Core::Crypto::Base64Decode(decryptResponse.plaintext));
        BOOST_CHECK_EQUAL(createResponse.keyMetadata.keyId, decryptResponse.keyId);
    }

    BOOST_AUTO_TEST_CASE(RsaEncryptDecryptTest) {

        // arrange
        const Service::KMSService kmsService;
        const Dto::KMS::CreateKeyResponse createResponse = CreateDefaultRsaKey(kmsService);
        BOOST_CHECK_EQUAL(false, createResponse.keyMetadata.keyId.empty());

        // wait for async RSA key material creation
        kmsService.WaitForRsaKey(createResponse.keyMetadata.keyId, 30);

        Dto::KMS::EncryptRequest encryptRequest;
        encryptRequest.region = TEST_REGION;
        encryptRequest.keyId = createResponse.keyMetadata.keyId;
        encryptRequest.plaintext = Core::Crypto::Base64Encode(std::string(TEST_PLAINTEXT));
        encryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::RSAES_OAEP_SHA_256;

        // act encrypt
        const Dto::KMS::EncryptResponse encryptResponse = kmsService.Encrypt(encryptRequest);

        // assert encrypt
        BOOST_CHECK_EQUAL(false, encryptResponse.ciphertext.empty());
        BOOST_CHECK_EQUAL(createResponse.keyMetadata.keyId, encryptResponse.keyId);

        // act decrypt
        Dto::KMS::DecryptRequest decryptRequest;
        decryptRequest.region = TEST_REGION;
        decryptRequest.keyId = createResponse.keyMetadata.keyId;
        decryptRequest.ciphertext = encryptResponse.ciphertext;
        decryptRequest.encryptionAlgorithm = Dto::KMS::EncryptionAlgorithm::RSAES_OAEP_SHA_256;

        const Dto::KMS::DecryptResponse decryptResponse = kmsService.Decrypt(decryptRequest);

        // assert decrypt
        BOOST_CHECK_EQUAL(std::string(TEST_PLAINTEXT), Core::Crypto::Base64Decode(decryptResponse.plaintext));
        BOOST_CHECK_EQUAL(createResponse.keyMetadata.keyId, decryptResponse.keyId);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
