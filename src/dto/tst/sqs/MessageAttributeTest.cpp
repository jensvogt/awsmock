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
#include "awsmock/utils/SqsUtils.h"


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
        Database::Entity::SQS::MessageAttribute attribute1;
        attribute1.stringValue = "my_attribute_value_1";
        attribute1.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttribute attribute2;
        attribute2.stringValue = "my_attribute_value_2";
        attribute2.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttributeList messageAttributes;
        messageAttributes["my_attribute_name_1"] = attribute1;
        messageAttributes["my_attribute_name_2"] = attribute2;

        // act
        const std::string result = Database::SqsUtils::CreateMd5OfMessageAttributes(messageAttributes);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_STREQ(result.c_str(), MD5_RESULT);
    }

    TEST_F(MessageAttributeTest, AttributeMd51Test) {

        // arrange
        Database::Entity::SQS::MessageAttribute attribute1;
        attribute1.stringValue = "application/json";
        attribute1.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttribute attribute2;
        attribute2.stringValue = "[{\"verarbeitungsschritt\":\"PARSEN\",\"internalId\":\"92e236b3-63ed-4582-820c-a308a4176c41\",\"queueName\":\"produktmeldung-retry-queue\"},{\"verarbeitungsschritt\":\"ZERLEGEN\",\"internalId\":\"ftpuser1/pim-3719_23012025133759888.xml\",\"queueName\":\"originalmeldung-retry-queue\"}]";
        attribute2.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttributeList messageAttributes;
        messageAttributes["contentType"] = attribute1;
        messageAttributes["retryContext"] = attribute2;

        // act
        const std::string result = Database::SqsUtils::CreateMd5OfMessageAttributes(messageAttributes);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_STREQ(result.c_str(), MD5_RESULT_1);
    }

    TEST_F(MessageAttributeTest, AttributeMd52Test) {

        // arrange
        Database::Entity::SQS::MessageAttribute attribute1;
        attribute1.stringValue = "application/json";
        attribute1.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttribute attribute2;
        attribute2.stringValue = "1737825223262";
        attribute1.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttribute attribute3;
        attribute3.stringValue = "614a11ff-a4e8-af80-4923-e321ff0fd401";
        attribute3.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttributeList messageAttributes;
        messageAttributes["contentType"] = attribute1;
        messageAttributes["timestamp"] = attribute2;
        messageAttributes["id"] = attribute3;

        // act
        const std::string result = Database::SqsUtils::CreateMd5OfMessageAttributes(messageAttributes);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_STREQ(result.c_str(), MD5_RESULT_2);
    }
}// namespace AwsMock::Dto::SQS

#endif// AWMOCK_DTO_SQS_MESSAGE_ATTRBUTE_TEST_H
