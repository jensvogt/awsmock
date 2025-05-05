//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_SERVICE_SNS_SERVICE_TEST_H
#define AWMOCK_SERVICE_SNS_SERVICE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/sns/SNSService.h>
#include <awsmock/service/sqs/SQSService.h>

// AwsMOck includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define TOPIC "test-topic"
#define QUEUE "test-queue"
#define QUEUE_URL "http://localhost:4566/000000000000/test-queue"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define BODY_MD5 "bf54bf4281dc11635fcdd2d7d6c9e126"
#define OWNER "test-owner"

namespace AwsMock::Service {

    /**
     * @brief Test the SNS service layer
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSServiceTest : public testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
            long deleted = _snsDatabase.DeleteAllMessages();
            log_info << "SNS message deleted, count: " << deleted;
            deleted = _snsDatabase.DeleteAllTopics();
            log_info << "SNS topics deleted, count: " << deleted;
            deleted = _sqsDatabase.DeleteAllMessages();
            log_info << "SQS message deleted, count: " << deleted;
            deleted = _sqsDatabase.DeleteAllQueues();
            log_info << "SQS queues deleted, count: " << deleted;
        }

        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
        SNSService _snsService;
        SQSService _sqsService;
    };

    TEST_F(SNSServiceTest, TopicCreateTest) {

        // arrange
        const Dto::SNS::CreateTopicRequest request = {.region = REGION, .topicName = TOPIC, .owner = OWNER};

        // act
        const Dto::SNS::CreateTopicResponse response = _snsService.CreateTopic(request);

        // assert
        EXPECT_TRUE(response.region == REGION);
        EXPECT_TRUE(response.name == TOPIC);
    }

    TEST_F(SNSServiceTest, TopicListTest) {

        // arrange
        const Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        const Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);

        // act
        const Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);

        // assert
        EXPECT_EQ(1, response.topicList.size());
        EXPECT_TRUE(response.topicList.front().region == REGION);
        EXPECT_TRUE(response.topicList.front().topicName == TOPIC);
    }

    TEST_F(SNSServiceTest, TopicPurgeTest) {

        // arrange
        const Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        const Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);
        const Dto::SNS::PublishRequest request = {.region = REGION, .topicArn = topicResponse.topicArn, .message = BODY};
        const auto [messageId, requestId] = _snsService.Publish(request);
        EXPECT_TRUE(messageId.length() > 1);
        const Dto::SNS::PurgeTopicRequest purgeRequest = {.topicArn = topicResponse.topicArn};

        // act
        long count = _snsService.PurgeTopic(purgeRequest);

        // assert
        EXPECT_EQ(1, count);
    }

    TEST_F(SNSServiceTest, TopicDeleteTest) {

        // arrange
        const Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        const Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);

        // act
        EXPECT_NO_THROW({
            auto [requestId] = _snsService.DeleteTopic(topicResponse.region, topicResponse.topicArn);
            EXPECT_FALSE(requestId.empty());
        });

        // assert
        EXPECT_EQ(0, _snsDatabase.ListTopics(REGION).size());
    }

    TEST_F(SNSServiceTest, SubscribeTest) {

        // arrange
        Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);
        Dto::SQS::CreateQueueRequest queueRequest = {.queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER};
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);

        // act
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);

        // assert
        EXPECT_FALSE(subscribeResponse.subscriptionArn.empty());
        EXPECT_EQ(1, response.topicList[0].subscriptions.size());
    }

    TEST_F(SNSServiceTest, SubscriptionUpdateTest) {

        // arrange
        Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);
        Dto::SQS::CreateQueueRequest queueRequest = {.queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER};
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);
        Dto::SNS::UpdateSubscriptionRequest updateRequest = {.topicArn = topicResponse.topicArn, .subscriptionArn = subscribeResponse.subscriptionArn, .protocol = "SQS", .endpoint = "foobar", .owner = "bar"};

        // act
        auto [subscriptionArn1] = _snsService.UpdateSubscription(updateRequest);

        // assert
        EXPECT_FALSE(subscribeResponse.subscriptionArn.empty());
    }

    TEST_F(SNSServiceTest, SubscriptionListTest) {

        // arrange
        Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);
        Dto::SQS::CreateQueueRequest queueRequest = {.queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER};
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::ListSubscriptionsByTopicRequest listRequest = {.region = REGION, .topicArn = topicResponse.topicArn};

        // act
        Dto::SNS::ListSubscriptionsByTopicResponse listResponse = _snsService.ListSubscriptionsByTopic(listRequest);

        // assert
        EXPECT_FALSE(listResponse.subscriptions.empty());
    }

    TEST_F(SNSServiceTest, UnsubscribeTest) {

        // arrange
        Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};

        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);

        // act
        Dto::SNS::UnsubscribeRequest unsubscribeRequest;
        unsubscribeRequest.region = REGION;
        unsubscribeRequest.subscriptionArn = subscribeResponse.subscriptionArn;
        Dto::SNS::UnsubscribeResponse unsubscribeResponse = _snsService.Unsubscribe(unsubscribeRequest);
        Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);

        // assert
        EXPECT_FALSE(subscribeResponse.subscriptionArn.empty());
        EXPECT_EQ(0, response.topicList[0].subscriptions.size());
    }

    TEST_F(SNSServiceTest, PublishMessageTest) {

        // arrange
        Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);
        Dto::SNS::PublishRequest request = {.region = REGION, .topicArn = topicResponse.topicArn, .message = BODY};

        // act
        Dto::SNS::PublishResponse response = _snsService.Publish(request);

        // assert
        EXPECT_FALSE(response.messageId.empty());
        EXPECT_FALSE(response.ToXml().empty());
    }

    TEST_F(SNSServiceTest, MessageReceiveTest) {

        // arrange
        Dto::SNS::CreateTopicRequest topicRequest = {.region = REGION, .topicName = TOPIC, .owner = OWNER};
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(topicRequest);
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};

        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);

        Dto::SQS::GetQueueUrlRequest queueUrlRequest = {.region = REGION, .queueName = QUEUE};
        auto [queueUrl] = _sqsService.GetQueueUrl(queueUrlRequest);
        std::string resultQueueUrl = queueUrl;

        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::PublishRequest request = {.region = REGION, .topicArn = topicResponse.topicArn, .message = BODY};
        Dto::SNS::PublishResponse response = _snsService.Publish(request);

        // act
        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = REGION;
        receiveRequest.queueUrl = resultQueueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 5;
        Dto::SQS::ReceiveMessageResponse receiveResponse = _sqsService.ReceiveMessages(receiveRequest);

        // assert
        EXPECT_EQ(receiveResponse.messageList.size(), 1);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_SNS_SERVICE_TEST_H