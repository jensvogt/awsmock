//
// Created by vogje01 on 12/18/23.
//

#ifndef AWMOCK_DTO_TRANSFER_CREATE_SERVER_REQUEST_TEST_H
#define AWMOCK_DTO_TRANSFER_CREATE_SERVER_REQUEST_TEST_H

// GTest includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/dto/transfer/CreateServerRequest.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define CREATE_SERVER_REQUEST_TO_STRING "CreateServerRequest={ \"Region\" : \"eu-central-1\", \"Domain\" : \"test.com\" }"
#define CREATE_SERVER_REQUEST_TO_JSON "{ \"Region\" : \"eu-central-1\", \"Domain\" : \"test.com\" }"
#define CREATE_SERVER_REQUEST_FROM_JSON "{\"Domain\":\"test.com\",\"Region\":\"eu-central-1\", \"Protocols\":[\"ftp\", \"sftp\"], \"tags\":[{\"key\":\"value\"}]}"

namespace AwsMock::Dto::Transfer {

    class CreateServerRequestTest : public ::testing::Test {
      protected:

        void SetUp() override {
            // General configuration
            _region = Core::Configuration::instance().GetValueString("awsmock.region");
        }

        std::string _region;
        std::vector<ProtocolType> _protocols = {ProtocolTypeFromString("FTP"), ProtocolTypeFromString("SFTP")};
        IdentityProviderDetails _identityProviderDetails = {.directoryId = "directoryId", .function = "function", .invocationRole = "invocationRole", .sftpAuthenticationMethod = SftpAuthenticationMethod::PASSWORD, .url = "url"};
        std::map<std::string, std::string> _tags = {{"version", "1.0"}};
    };

    TEST_F(CreateServerRequestTest, ToStringTest) {

        // arrange
        const CreateServerRequest createRequest = {.region = _region, .domain = "test.com", .protocols = _protocols, .tags = _tags, .identityProviderDetails = _identityProviderDetails};

        // act
        const std::string stringRepresentation = createRequest.ToString();

        // assert
        EXPECT_FALSE(stringRepresentation.empty());
        EXPECT_TRUE(Core::StringUtils::Equals(stringRepresentation, CREATE_SERVER_REQUEST_TO_STRING));
    }

    TEST_F(CreateServerRequestTest, ToJsonTest) {

        // arrange
        const CreateServerRequest createRequest = {.region = _region, .domain = "test.com", .protocols = _protocols, .tags = _tags, .identityProviderDetails = _identityProviderDetails};

        // act
        const std::string jsonRepresentation = createRequest.ToJson();

        // assert
        EXPECT_FALSE(jsonRepresentation.empty());
        EXPECT_TRUE(Core::StringUtils::Equals(jsonRepresentation, CREATE_SERVER_REQUEST_TO_JSON));
    }

    TEST_F(CreateServerRequestTest, FromJsonTest) {

        // arrange
        CreateServerRequest createRequest;
        const std::string jsonRepresentation = CREATE_SERVER_REQUEST_FROM_JSON;

        // act
        createRequest.FromJson(jsonRepresentation);

        // assert
        EXPECT_TRUE(createRequest.region == _region);
        EXPECT_TRUE(createRequest.domain == "test.com");
        //    ASSERT_THAT(createRequest.protocols, testing::ElementsAre("ftp", "sftp"));
        for (const auto &[key, value]: createRequest.tags) {
            EXPECT_TRUE(key == "key");
            EXPECT_TRUE(value == "value");
        }
    }
}// namespace AwsMock::Dto::Transfer

#endif// AWMOCK_DTO_TRANSFER_CREATE_SERVER_REQUEST_TEST_H
