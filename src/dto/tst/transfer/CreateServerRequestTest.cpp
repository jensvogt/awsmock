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
#include "awsmock/dto/transfer/model/Tag.h"


#include <awsmock/core/TestUtils.h>

#define CREATE_SERVER_REQUEST_FROM_JSON "{\"Domain\":\"test.com\",\"region\":\"eu-central-1\", \"Protocols\":[\"ftp\", \"sftp\"], \"Tags\":{\"key\":\"value\"}}"

namespace AwsMock::Dto::Transfer {

    class CreateServerRequestTest : public ::testing::Test {
      protected:

        void SetUp() override {
            // General configuration
            _region = Core::Configuration::instance().GetValue<std::string>("awsmock.region");
            _identityProviderDetails.directoryId = "directoryId";
            _identityProviderDetails.function = "function";
            _identityProviderDetails.invocationRole = "invocationRole";
            _identityProviderDetails.sftpAuthenticationMethod = SftpAuthenticationMethod::PASSWORD;
            _identityProviderDetails.url = "url";
            _tags["version"] = "1.0";
        }

        std::string _region;
        std::vector<ProtocolType> _protocols = {ProtocolTypeFromString("FTP"), ProtocolTypeFromString("SFTP")};
        IdentityProviderDetails _identityProviderDetails;
        std::map<std::string, std::string> _tags;
    };

    TEST_F(CreateServerRequestTest, ToStringTest) {

        // arrange
        CreateServerRequest createRequest;
        createRequest.region = _region;
        createRequest.domain = "test.com";
        createRequest.protocols = _protocols;
        createRequest.tags = _tags;
        createRequest.identityProviderDetails = _identityProviderDetails;

        // act
        const std::string stringRepresentation = createRequest.ToString();

        // assert
        EXPECT_FALSE(stringRepresentation.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(stringRepresentation, _region));
        EXPECT_TRUE(Core::StringUtils::Contains(stringRepresentation, "RequestId"));
        EXPECT_TRUE(Core::StringUtils::Contains(stringRepresentation, "User"));
        EXPECT_TRUE(Core::StringUtils::Contains(stringRepresentation, "test.com"));
    }

    TEST_F(CreateServerRequestTest, ToJsonTest) {

        // arrange
        CreateServerRequest createRequest;
        createRequest.region = _region;
        createRequest.domain = "test.com";
        createRequest.protocols = _protocols;
        createRequest.tags = _tags;
        createRequest.identityProviderDetails = _identityProviderDetails;

        // act
        const std::string jsonRepresentation = createRequest.ToJson();

        // assert
        EXPECT_FALSE(jsonRepresentation.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(jsonRepresentation, _region));
        EXPECT_TRUE(Core::StringUtils::Contains(jsonRepresentation, "RequestId"));
        EXPECT_TRUE(Core::StringUtils::Contains(jsonRepresentation, "User"));
        EXPECT_TRUE(Core::StringUtils::Contains(jsonRepresentation, "test.com"));
    }

    TEST_F(CreateServerRequestTest, FromJsonTest) {

        // arrange
        const std::string jsonRepresentation = CREATE_SERVER_REQUEST_FROM_JSON;

        // act
        CreateServerRequest createRequest = CreateServerRequest::FromJson(jsonRepresentation);

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
