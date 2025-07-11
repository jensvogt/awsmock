//
// Created by vogje01 on 02/06/2023.
//

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/kms/KMSService.h>

// Test includes
#include "TestBase.h"
#include <awsmock/core/TestUtils.h>

#define BOOST_TEST_MODULE KMSServiceCliTests
#define PLAIN_TEXT_BASE64 std::string("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==")

namespace AwsMock::Service {

    /**
     * @brief AwsMock cognito integration test.
     */
    struct KMSServiceCliTests : TestBase {

        KMSServiceCliTests() {

            // Start the gateway server
            StartGateway();

            // General configuration
            _region = GetRegion();
            _endpoint = GetEndpoint();

            // ReSharper disable once CppExpressionWithoutSideEffects
            _database.DeleteAllKeys();
        }

        std::string _endpoint, _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        Database::KMSDatabase &_database = Database::KMSDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(KeyCreateSymmetricTest, KMSServiceCliTests) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "SYMMETRIC_DEFAULT", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        BOOST_CHECK_EQUAL(keyList.empty(), false);
        BOOST_CHECK_EQUAL(keyList.at(0).arn.empty(), false);
        BOOST_CHECK_EQUAL(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::SYMMETRIC_DEFAULT), true);
        BOOST_CHECK_EQUAL(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT), true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyCreateRSA2048Test, KMSServiceCliTests) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_2048", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        BOOST_CHECK_EQUAL(1, keyList.size());
        BOOST_CHECK_EQUAL(keyList.at(0).arn.empty(), false);
        BOOST_CHECK_EQUAL(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::RSA_2048), true);
        BOOST_CHECK_EQUAL(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT), true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyCreateRSA3072Test, KMSServiceCliTests) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_3072", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        BOOST_CHECK_EQUAL(1, keyList.size());
        BOOST_CHECK_EQUAL(keyList.at(0).arn.empty(), false);
        BOOST_CHECK_EQUAL(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::RSA_3072), true);
        BOOST_CHECK_EQUAL(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT), true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyCreateRSA4096Test, KMSServiceCliTests) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_4096", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();

        // assert
        BOOST_CHECK_EQUAL(1, keyList.size());
        BOOST_CHECK_EQUAL(keyList.at(0).arn.empty(), false);
        BOOST_CHECK_EQUAL(keyList.at(0).keySpec == Dto::KMS::KeySpecToString(Dto::KMS::KeySpec::RSA_4096), true);
        BOOST_CHECK_EQUAL(keyList.at(0).keyUsage == Dto::KMS::KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT), true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyListTest, KMSServiceCliTests) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "RSA_4096", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "list-keys", "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(output2.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output2, "Keys"), true);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output2, "KeyId"), true);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output2, "KeyArn"), true);
    }

    BOOST_FIXTURE_TEST_CASE(EncryptTest, KMSServiceCliTests) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "SYMMETRIC_DEFAULT", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();
        const std::string keyId = keyList.at(0).keyId;

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "encrypt", "--key-id", keyId, "--plaintext", PLAIN_TEXT_BASE64, "--endpoint", _endpoint});
        const Dto::KMS::EncryptResponse response = Dto::KMS::EncryptResponse::FromJson(output2);

        // assert
        BOOST_CHECK_EQUAL(output2.empty(), false);
        BOOST_CHECK_EQUAL(response.ciphertext.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(DecryptTest, KMSServiceCliTests) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "create-key", "--key-spec", "SYMMETRIC_DEFAULT", "--key-usage", "ENCRYPT_DECRYPT", "--endpoint", _endpoint});
        const std::vector<Database::Entity::KMS::Key> keyList = _database.ListKeys();
        const std::string keyId = keyList.at(0).keyId;
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "encrypt", "--key-id", keyId, "--plaintext", PLAIN_TEXT_BASE64, "--endpoint", _endpoint});
        Dto::KMS::EncryptResponse response1 = Dto::KMS::EncryptResponse::FromJson(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"kms", "decrypt", "--key-id", keyId, "--ciphertext-blob", response1.ciphertext, "--endpoint", _endpoint});
        Dto::KMS::DecryptResponse response2 = Dto::KMS::DecryptResponse::FromJson(output3);

        // assert
        BOOST_CHECK_EQUAL(output3.empty(), false);
        BOOST_CHECK_EQUAL(response2.plaintext, PLAIN_TEXT_BASE64);
    }

}// namespace AwsMock::Service
