//
// Created by vogje01 on 21/10/2023.
//

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/repository/KMSDatabase.h>

#define BOOST_TEST_MODULE KMSServiceJavaTests
#define TEST_PORT 10100
#define REGION "eu-central-1"

namespace AwsMock::Service {

    /**
     * @brief Tests the aws-sdk-java interface to the AwsMock system.
     *
     * @par
     * The awsmock-test docker image will be started. The KMS server is started.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct KMSServiceJavaTest : TestBase {

        KMSServiceJavaTest() {

            // Start the gateway server
            StartGateway(TEST_PORT);

            // Base URL
            _kmsBaseUrl = "/api/kms/";

            // ReSharper disable once CppExpressionWithoutSideEffects
            _kmsDatabase.DeleteAllKeys();
        }

        std::string _kmsBaseUrl;
        Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(KeySymmetricTest, KMSServiceJavaTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createSymmetricKey?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::SYMMETRIC_DEFAULT) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        BOOST_CHECK_EQUAL(result.statusCode, http::status::ok);
        const Dto::KMS::Key response = Dto::KMS::Key::FromJson(result.body);

        // assert
        BOOST_CHECK_EQUAL(response.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.arn.empty(), false);
        BOOST_CHECK_EQUAL(response.keySpec == Dto::KMS::KeySpec::SYMMETRIC_DEFAULT, true);
        BOOST_CHECK_EQUAL(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT, true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyRSA2048Test, KMSServiceJavaTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_2048) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        BOOST_CHECK_EQUAL(result.statusCode, http::status::ok);
        Dto::KMS::Key response;
        response.FromJson(result.body);

        // assert
        BOOST_CHECK_EQUAL(response.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.arn.empty(), false);
        BOOST_CHECK_EQUAL(response.keySpec == Dto::KMS::KeySpec::RSA_2048, true);
        BOOST_CHECK_EQUAL(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT, true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyRSA3072Test, KMSServiceJavaTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_3072) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        BOOST_CHECK_EQUAL(result.statusCode, http::status::ok);
        Dto::KMS::Key response;
        response.FromJson(result.body);

        // assert
        BOOST_CHECK_EQUAL(response.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.arn.empty(), false);
        BOOST_CHECK_EQUAL(response.keySpec == Dto::KMS::KeySpec::RSA_3072, true);
        BOOST_CHECK_EQUAL(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT, true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyRSA4096Test, KMSServiceJavaTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_4096) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        BOOST_CHECK_EQUAL(result.statusCode, http::status::ok);
        Dto::KMS::Key response;
        response.FromJson(result.body);

        // assert
        BOOST_CHECK_EQUAL(response.keyId.empty(), false);
        BOOST_CHECK_EQUAL(response.arn.empty(), false);
        BOOST_CHECK_EQUAL(response.keySpec == Dto::KMS::KeySpec::RSA_4096, true);
        BOOST_CHECK_EQUAL(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT, true);
    }

    BOOST_FIXTURE_TEST_CASE(KeyListJTest, KMSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_2048) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        BOOST_CHECK_EQUAL(result.statusCode, http::status::ok);

        // act
        const Core::HttpSocketResponse listResult = SendPostCommand(_kmsBaseUrl + "listKeys?limit=10", {}, TEST_PORT);
        BOOST_CHECK_EQUAL(result.statusCode, http::status::ok);

        std::vector<Dto::KMS::Key> resultKeys;
        for (const value keys = bsoncxx::from_json(listResult.body); const auto &element: keys) {
            Dto::KMS::Key key;
            key.FromDocument(element.get_document().view());
            resultKeys.push_back(key);
        }

        // assert
        BOOST_CHECK_EQUAL(resultKeys.at(0).keyId.empty(), false);
        BOOST_CHECK_EQUAL(resultKeys.at(0).arn.empty(), false);
    }

}// namespace AwsMock::Service
