//
// Created by vogje01 on 21/10/2023.
//

#ifndef AWMOCK_SERVICE_KMS_JAVA_SERVER_TEST_H
#define AWMOCK_SERVICE_KMS_JAVA_SERVER_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/KMSDatabase.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/kms/KMSServer.h>

#define TEST_PORT 10100

namespace AwsMock::Service {

    /**
     * @brief Tests the aws-sdk-java interface to the AwsMock system.
     *
     * @par
     * The awsmock-test docker image will be started. The KMS server is started.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSServerJavaTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // Start gateway server
            StartGateway(TEST_PORT);

            // General configuration
            _region = GetRegion();

            // Base URL
            _kmsBaseUrl = "/api/kms/";
        }

        void TearDown() override {
            _kmsDatabase.DeleteAllKeys();
        }

        boost::thread _thread;
        std::string _kmsBaseUrl, _region;
        boost::asio::io_service _ios{10};
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
    };

    TEST_F(KMSServerJavaTest, KeySymmetricTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createSymmetricKey?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::SYMMETRIC_DEFAULT) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        ASSERT_EQ(result.statusCode, http::status::ok);
        Dto::KMS::Key response;
        response.FromJson(result.body);

        // assert
        EXPECT_FALSE(response.keyId.empty());
        EXPECT_FALSE(response.arn.empty());
        EXPECT_TRUE(response.keySpec == Dto::KMS::KeySpec::SYMMETRIC_DEFAULT);
        EXPECT_TRUE(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT);
    }

    TEST_F(KMSServerJavaTest, KeyRSA2048Test) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_2048) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        ASSERT_EQ(result.statusCode, http::status::ok);
        Dto::KMS::Key response;
        response.FromJson(result.body);

        // assert
        EXPECT_FALSE(response.keyId.empty());
        EXPECT_FALSE(response.arn.empty());
        EXPECT_TRUE(response.keySpec == Dto::KMS::KeySpec::RSA_2048);
        EXPECT_TRUE(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT);
    }

    TEST_F(KMSServerJavaTest, KeyRSA3072Test) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_3072) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        ASSERT_EQ(result.statusCode, http::status::ok);
        Dto::KMS::Key response;
        response.FromJson(result.body);

        // assert
        EXPECT_FALSE(response.keyId.empty());
        EXPECT_FALSE(response.arn.empty());
        EXPECT_TRUE(response.keySpec == Dto::KMS::KeySpec::RSA_3072);
        EXPECT_TRUE(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT);
    }

    TEST_F(KMSServerJavaTest, KeyRSA4096Test) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_4096) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        ASSERT_EQ(result.statusCode, http::status::ok);
        Dto::KMS::Key response;
        response.FromJson(result.body);

        // assert
        EXPECT_FALSE(response.keyId.empty());
        EXPECT_FALSE(response.arn.empty());
        EXPECT_TRUE(response.keySpec == Dto::KMS::KeySpec::RSA_4096);
        EXPECT_TRUE(response.keyUsage == Dto::KMS::KeyUsage::ENCRYPT_DECRYPT);
    }

    TEST_F(KMSServerJavaTest, KeyListTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_kmsBaseUrl + "createRSA2048Key?keySpec=" + KeySpecToString(Dto::KMS::KeySpec::RSA_2048) + "&keyUsage=" + KeyUsageToString(Dto::KMS::KeyUsage::ENCRYPT_DECRYPT) + "&description=Description", {}, TEST_PORT);
        ASSERT_EQ(result.statusCode, http::status::ok);

        // act
        const Core::HttpSocketResponse listResult = SendPostCommand(_kmsBaseUrl + "listKeys?limit=10", {}, TEST_PORT);
        ASSERT_EQ(result.statusCode, http::status::ok);

        std::vector<Dto::KMS::Key> resultKeys;
        for (const value keys = bsoncxx::from_json(listResult.body); const auto &element: keys) {
            Dto::KMS::Key key;
            key.FromDocument(element.get_document().view());
            resultKeys.push_back(key);
        }

        // assert
        EXPECT_FALSE(resultKeys.at(0).keyId.empty());
        EXPECT_FALSE(resultKeys.at(0).arn.empty());
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_KMS_JAVA_SERVER_TEST_H