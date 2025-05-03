//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_SERVICE_SQS_SERVICE_TEST_H
#define AWMOCK_SERVICE_SQS_SERVICE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/sqs/SQSService.h>

// AwsMOck includes
#include "awsmock/utils/SqsUtils.h"


#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define QUEUE "test-queue"
#define QUEUE_URL "http://localhost:4566/000000000000/test-queue"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define BODY_MD5 "bf54bf4281dc11635fcdd2d7d6c9e126"
#define EMPTY_MD5 "d41d8cd98f00b204e9800998ecf8427e"
#define OWNER "test-owner"

namespace AwsMock::Service {

    /**
     * @brief Test the SQS service layer
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSServiceTest : public testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
            const long queueCount = _database.DeleteAllQueues();
            const long messageCount = _database.DeleteAllMessages();
            log_debug << "Queue deleted: " << queueCount << " messages: " << messageCount;
        }

        Database::SQSDatabase &_database = Database::SQSDatabase::instance();
        SQSService _service;
    };

    TEST_F(SQSServiceTest, QueueCreateTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};

        // act
        Dto::SQS::CreateQueueResponse response = _service.CreateQueue(request);

        // assert
        EXPECT_TRUE(response.name == QUEUE);
        EXPECT_TRUE(response.region == REGION);
    }

    TEST_F(SQSServiceTest, QueueListTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(request);
        Dto::SQS::ListQueuesRequest listQueuesRequest = {.region = REGION, .maxResults = 100, .nextToken = ""};

        // act
        Dto::SQS::ListQueuesResponse response = _service.ListQueues(listQueuesRequest);

        // assert
        EXPECT_FALSE(response.queueList.empty());
    }

    TEST_F(SQSServiceTest, QueueListEmptyTest) {

        // arrange
        const Dto::SQS::ListQueuesRequest listQueuesRequest = {.region = REGION, .maxResults = 100, .nextToken = ""};

        // act
        const Dto::SQS::ListQueuesResponse response = _service.ListQueues(listQueuesRequest);

        // assert
        EXPECT_TRUE(response.queueList.empty());
    }

    TEST_F(SQSServiceTest, QueueNotExistenceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(request);
        Dto::SQS::GetQueueDetailsRequest getQueueDetailsRequest;
        getQueueDetailsRequest.region = REGION;
        getQueueDetailsRequest.queueArn = "notExist";

        // act
        EXPECT_THROW({
        try {
            Dto::SQS::GetQueueDetailsResponse response = _service.GetQueueDetails(getQueueDetailsRequest);
        } catch( const Core::ServiceException& e ) {
            // and this tests that it has the correct message
            EXPECT_STREQ( "Queue does not exist, queueArn: notExist", e.what() );
            throw;} }, Core::ServiceException);
    }

    TEST_F(SQSServiceTest, QueueGetDetailsTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::SendMessageRequest sendMessageRequest = {.region = REGION, .queueUrl = queueResponse.queueUrl, .body = BODY, .messageId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::SendMessageResponse sendMessageResponse = _service.SendMessage(sendMessageRequest);
        Dto::SQS::GetQueueDetailsRequest getQueueDetailsRequest;
        getQueueDetailsRequest.region = REGION;
        getQueueDetailsRequest.queueArn = queueResponse.queueArn;

        // act
        Dto::SQS::GetQueueDetailsResponse response = _service.GetQueueDetails(getQueueDetailsRequest);

        // assert
        EXPECT_EQ(1, response.available);
        EXPECT_EQ(0, response.delayed);
        EXPECT_EQ(0, response.invisible);
    }

    TEST_F(SQSServiceTest, QueueListArnsTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);

        // act
        auto [queueArns] = _service.ListQueueArns();

        // assert
        EXPECT_EQ(1, queueArns.size());
        EXPECT_TRUE(queueArns.at(0) == queueResponse.queueArn);
    }

    TEST_F(SQSServiceTest, QueueListCountersTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::SendMessageRequest sendMessageRequest = {.region = REGION, .queueUrl = queueResponse.queueUrl, .body = BODY, .messageId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::SendMessageResponse sendMessageResponse = _service.SendMessage(sendMessageRequest);
        Dto::SQS::ListQueueCountersRequest listQueueCountersRequest;
        listQueueCountersRequest.region = REGION;

        // act
        Dto::SQS::ListQueueCountersResponse response = _service.ListQueueCounters(listQueueCountersRequest);

        // assert
        EXPECT_EQ(1, response.total);
        EXPECT_EQ(1, response.queueCounters.at(0).available);
    }

    TEST_F(SQSServiceTest, QueueListTagsTest) {

    // arrange
    std:
        std::map<std::string, std::string> inputTags = {{"version", "1.0"}};
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER, .tags = inputTags};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::ListQueueTagsRequest listQueueTagsRequest = {.region = REGION, .queueUrl = queueResponse.queueUrl};

        // act
        auto [tags, total] = _service.ListQueueTags(listQueueTagsRequest);

        // assert
        EXPECT_EQ(1, total);
        EXPECT_EQ(1, tags.size());
        EXPECT_TRUE(tags["version"] == "1.0");
    }

    TEST_F(SQSServiceTest, QueueDeleteTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);

        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;
        Dto::SQS::DeleteQueueRequest deleteRequest = {.region = REGION, .queueUrl = queueUrl};
        deleteRequest.requestId = Core::StringUtils::CreateRandomUuid();

        // act
        EXPECT_NO_THROW({
            Dto::SQS::DeleteQueueResponse response = _service.DeleteQueue(deleteRequest);
        });

        // assert
        EXPECT_EQ(0, _database.ListQueues(REGION).size());
    }

    TEST_F(SQSServiceTest, MessageCreateTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER};
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();

        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest request = {.queueUrl = queueUrl, .body = BODY, .messageId = Core::StringUtils::CreateRandomUuid()};
        request.region = REGION;
        request.requestId = Core::StringUtils::CreateRandomUuid();

        // act
        Dto::SQS::SendMessageResponse response = _service.SendMessage(request);

        // assert
        EXPECT_FALSE(response.messageId.empty());
        EXPECT_TRUE(response.md5Body == BODY_MD5);
        EXPECT_TRUE(response.md5MessageAttributes == EMPTY_MD5);
        EXPECT_TRUE(response.md5SystemAttributes.empty());
    }

    TEST_F(SQSServiceTest, MessagesCreateTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER};
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();

        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest request1 = {.region = REGION, .queueUrl = queueUrl, .body = BODY, .messageId = Core::StringUtils::CreateRandomUuid()};
        request1.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::SendMessageRequest request2 = {.region = REGION, .queueUrl = queueUrl, .body = BODY, .messageId = Core::StringUtils::CreateRandomUuid()};
        request2.requestId = Core::StringUtils::CreateRandomUuid();

        // act
        Dto::SQS::SendMessageResponse response1 = _service.SendMessage(request1);
        Dto::SQS::SendMessageResponse response2 = _service.SendMessage(request2);

        // assert
        EXPECT_FALSE(response1.messageId.empty());
        EXPECT_TRUE(response1.md5Body == BODY_MD5);
        EXPECT_TRUE(response1.md5MessageAttributes == EMPTY_MD5);
        EXPECT_TRUE(response1.md5SystemAttributes.empty());
        EXPECT_FALSE(response2.messageId.empty());
        EXPECT_TRUE(response2.md5Body == BODY_MD5);
        EXPECT_TRUE(response2.md5MessageAttributes == EMPTY_MD5);
        EXPECT_TRUE(response2.md5SystemAttributes.empty());
    }

    TEST_F(SQSServiceTest, MessageReceiveTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest msgRequest = {.region = REGION, .queueUrl = queueUrl, .queueName = QUEUE, .body = BODY, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::SendMessageResponse msgResponse = _service.SendMessage(msgRequest);

        // act
        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = REGION;
        receiveRequest.queueUrl = queueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 1;
        Dto::SQS::ReceiveMessageResponse receiveResponse = _service.ReceiveMessages(receiveRequest);

        // assert
        EXPECT_EQ(receiveResponse.messageList.size(), 1);
    }

    TEST_F(SQSServiceTest, MessageDeleteTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest msgRequest = {.region = REGION, .queueUrl = queueUrl, .body = BODY, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::SendMessageResponse msgResponse = _service.SendMessage(msgRequest);

        // act
        Dto::SQS::DeleteMessageRequest delRequest = {.queueUrl = queueUrl, .receiptHandle = msgResponse.receiptHandle};
        Dto::SQS::DeleteMessageResponse delResponse;
        EXPECT_NO_FATAL_FAILURE({ _service.DeleteMessage(delRequest); });

        // assert
        EXPECT_EQ(0, _database.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl)));
    }

    TEST_F(SQSServiceTest, GetMd5AttributesTest) {

        // arrange
        //
        // MessageAttribute.1.Name=contentType
        // MessageAttribute.1.Value.StringValue=application/json
        // MessageAttribute.1.Value.DataType=String
        //
        Database::Entity::SQS::MessageAttribute messageAttribute;
        messageAttribute.stringValue = "application/json";
        messageAttribute.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        std::map<std::string, Database::Entity::SQS::MessageAttribute> messageAttributes;
        messageAttributes["contentType"] = messageAttribute;

        // act
        const std::string md5sum = Database::SqsUtils::CreateMd5OfMessageAttributes(messageAttributes);

        // assert
        EXPECT_TRUE("6ed5f16969b625c8d900cbd5da557e9e" == md5sum);
    }

    TEST_F(SQSServiceTest, GetMd5AttributeListTest) {

        // arrange
        //
        // MessageAttribute.1.Name=contentType
        // MessageAttribute.1.Value.StringValue=application/json
        // MessageAttribute.1.Value.DataType=String
        // MessageAttribute.2.Name=contentLength
        // MessageAttribute.2.Value.StringValue=42
        // MessageAttribute.2.Value.DataType=Number
        //
        Database::Entity::SQS::MessageAttribute messageAttribute1;
        std::string name1 = "contentType";
        messageAttribute1.stringValue = "application/json";
        messageAttribute1.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttribute messageAttribute2;
        std::string name2 = "contentType";
        messageAttribute2.stringValue = "42";
        messageAttribute2.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        std::map<std::string, Database::Entity::SQS::MessageAttribute> messageAttributes;
        messageAttributes[name1] = messageAttribute1;
        messageAttributes[name2] = messageAttribute2;

        // act
        const std::string md5sum = Database::SqsUtils::CreateMd5OfMessageAttributes(messageAttributes);

        // assert
        EXPECT_TRUE("ebade6c58059dfd4bbf8cee9da7465fe" == md5sum);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_SQS_SERVICE_TEST_H