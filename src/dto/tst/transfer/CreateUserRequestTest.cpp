//
// Created by vogje01 on 12/18/23.
//

#ifndef AWMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_TEST_H
#define AWMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_TEST_H

// C++ includes
#include <sstream>
#include <string>

// GTest includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/dto/transfer/CreateUserRequest.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define CREATE_USER_REQUEST_TO_STRING "CreateUserRequest={ \"Region\" : \"eu-central-1\", \"ServerId\" : \"test-server_id\", \"UserName\" : \"test-user\", \"Password\" : \"password\", \"HomeDirectory\" : \"test-user\" }"
#define CREATE_USER_REQUEST_TO_JSON "{ \"Region\" : \"eu-central-1\", \"ServerId\" : \"test-server_id\", \"UserName\" : \"test-user\", \"Password\" : \"password\", \"HomeDirectory\" : \"test-user\" }"
#define CREATE_USER_REQUEST_FROM_JSON "{ \"Region\" : \"eu-central-1\", \"ServerId\" : \"test-server_id\", \"UserName\" : \"test-user\", \"Password\" : \"password\", \"HomeDirectory\" : \"test-user\" }"

namespace AwsMock::Dto::Transfer {

    class CreateUserRequestTest : public ::testing::Test {
      protected:

        void SetUp() override {
            // General configuration
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        void TearDown() override {
        }

        std::string _region;
        Core::Configuration _configuration = Core::Configuration(TMP_CONFIGURATION_FILE);
    };

    TEST_F(CreateUserRequestTest, ToStringTest) {

        // arrange
        const CreateUserRequest createRequest = {.region = _region, .serverId = "test-server_id", .userName = "test-user", .password = "password", .homeDirectory = "test-user"};

        // act
        const std::string stringRepresentation = createRequest.ToString();

        // assert
        EXPECT_FALSE(stringRepresentation.empty());
        EXPECT_STREQ(stringRepresentation.c_str(), CREATE_USER_REQUEST_TO_STRING);
    }

    TEST_F(CreateUserRequestTest, ToJsonTest) {

        // arrange
        const CreateUserRequest createRequest = {.region = _region, .serverId = "test-server_id", .userName = "test-user", .password = "password", .homeDirectory = "test-user"};

        // act
        const std::string jsonRepresentation = createRequest.ToJson();

        // assert
        EXPECT_FALSE(jsonRepresentation.empty());
        EXPECT_TRUE(jsonRepresentation == CREATE_USER_REQUEST_TO_JSON);
    }

    TEST_F(CreateUserRequestTest, FromJsonTest) {

        // arrange
        CreateUserRequest createRequest;
        const std::string jsonRepresentation = CREATE_USER_REQUEST_FROM_JSON;

        // act
        createRequest.FromJson(jsonRepresentation);

        // assert
        EXPECT_TRUE(createRequest.region == _region);
        EXPECT_TRUE(createRequest.serverId == "test-server_id");
        ASSERT_THAT(createRequest.userName, "test-user");
    }
}// namespace AwsMock::Dto::Transfer

#endif// AWMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_TEST_H
