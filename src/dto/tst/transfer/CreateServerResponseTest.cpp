//
// Created by vogje01 on 12/18/23.
//

#ifndef AWMOCK_DTO_TRANSFER_CREATE_SERVER_RESPONSE_TEST_H
#define AWMOCK_DTO_TRANSFER_CREATE_SERVER_RESPONSE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/dto/transfer/CreateServerResponse.h>
#include <awsmock/dto/transfer/model/Tag.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define CREATE_SERVER_RESPONSE_TO_STRING "CreateServerResponse={ \"Region\" : \"eu-central-1\", \"ServerId\" : \"serverId\", \"Arn\" : \"arn\" }"
#define CREATE_SERVER_RESPONSE_TO_JSON "{ \"Region\" : \"eu-central-1\", \"ServerId\" : \"serverId\", \"Arn\" : \"arn\" }"

namespace AwsMock::Dto::Transfer {

    class CreateServerResponseTest : public ::testing::Test {
      protected:

        void SetUp() override {
            // General configuration
            _region = _configuration.GetValue<std::string>("awsmock.region");
            Tag tag;
            tag.key = "version";
            tag.value = "1.0";
            _tags.emplace_back();
        }

        std::string _region;
        std::vector<std::string> _protocols = {"ftp", "sftp"};
        std::vector<Tag> _tags;
        Core::Configuration _configuration = Core::Configuration(TMP_CONFIGURATION_FILE);
        boost::json::serializer()
    };

    TEST_F(CreateServerResponseTest, ToStringTest) {

        // arrange
        CreateServerResponse createResponse;
        createResponse.region = _region;
        createResponse.serverId = "serverId";
        createResponse.arn = "arn";

        // act
        const std::string stringRepresentation = createResponse.ToString();

        // assert
        EXPECT_FALSE(stringRepresentation.empty());
        EXPECT_STREQ(stringRepresentation.c_str(), CREATE_SERVER_RESPONSE_TO_STRING);
    }

    TEST_F(CreateServerResponseTest, ToJsonTest) {

        // arrange
        CreateServerResponse createResponse;
        createResponse.region = _region;
        createResponse.serverId = "serverId";
        createResponse.arn = "arn";

        // act
        const std::string jsonRepresentation = createResponse.ToJson();

        // assert
        EXPECT_FALSE(jsonRepresentation.empty());
        EXPECT_STREQ(jsonRepresentation.c_str(), CREATE_SERVER_RESPONSE_TO_JSON);
    }
}// namespace AwsMock::Dto::Transfer

#endif// AWMOCK_DTO_TRANSFER_CREATE_SERVER_RESPONSE_TEST_H
