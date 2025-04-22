//
// Created by vogje01 on 12/18/23.
//

#ifndef AWMOCK_DTO_SQS_MESSAGE_ATTRBUTE_TEST_H
#define AWMOCK_DTO_SQS_MESSAGE_ATTRBUTE_TEST_H

// GTest includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/dto/sqs/ReceiveMessageRequest.h>
#include <awsmock/dto/sqs/ReceiveMessageResponse.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define MD5_RESULT "c48838208d2b4e14e3ca0093a8443f09"
#define MD5_RESULT_1 "50997ba07d335ea661e138bd63a61fdc"
#define MD5_RESULT_2 "4f3a21e18018c2cab67b8f8cc72baeda"

namespace AwsMock::Dto::SQS {

    class MessageAttributeTest : public ::testing::Test {
      protected:

        void SetUp() override {
        }

        void TearDown() override {
        }
    };

    TEST_F(MessageAttributeTest, AttributeMd5Test) {

        // arrange
        MessageAttributeList messageAttributes;
        messageAttributes["my_attribute_name_1"] = {.stringValue = "my_attribute_value_1", .type = STRING};
        messageAttributes["my_attribute_name_2"] = {.stringValue = "my_attribute_value_2", .type = STRING};
        ReceiveMessageResponse response;
        response.messageAttributes = messageAttributes;

        // act
        const std::string result = MessageAttribute::GetMd5Attributes(messageAttributes);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_STREQ(result.c_str(), MD5_RESULT);
    }

    TEST_F(MessageAttributeTest, AttributeMd51Test) {

        // arrange
        MessageAttributeList messageAttributes;
        messageAttributes["contentType"] = {.stringValue = "application/json", .type = STRING};
        messageAttributes["retryContext"] = {.stringValue = "[{\"verarbeitungsschritt\":\"PARSEN\",\"internalId\":\"92e236b3-63ed-4582-820c-a308a4176c41\",\"queueName\":\"produktmeldung-retry-queue\"},{\"verarbeitungsschritt\":\"ZERLEGEN\",\"internalId\":\"ftpuser1/pim-3719_23012025133759888.xml\",\"queueName\":\"originalmeldung-retry-queue\"}]", .type = STRING};
        ReceiveMessageResponse response;
        response.messageAttributes = messageAttributes;

        // act
        const std::string result = MessageAttribute::GetMd5Attributes(messageAttributes);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_STREQ(result.c_str(), MD5_RESULT_1);
    }

    TEST_F(MessageAttributeTest, AttributeMd52Test) {

        // arrange
        MessageAttributeList messageAttributes;
        messageAttributes["contentType"] = {.stringValue = "application/json", .type = STRING};
        messageAttributes["timestamp"] = {.stringValue = "1737825223262", .type = STRING};
        messageAttributes["id"] = {.stringValue = "614a11ff-a4e8-af80-4923-e321ff0fd401", .type = STRING};
        ReceiveMessageResponse response;
        response.messageAttributes = messageAttributes;

        // act
        const std::string result = MessageAttribute::GetMd5Attributes(messageAttributes);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_STREQ(result.c_str(), MD5_RESULT_2);
    }
}// namespace AwsMock::Dto::SQS

#endif// AWMOCK_DTO_SQS_MESSAGE_ATTRBUTE_TEST_H
