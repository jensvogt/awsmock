//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SNS_DATABASE_TEST_H
#define AWMOCK_CORE_SNS_DATABASE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SNSDatabase.h>

#define REGION "eu-central-1"
#define TOPIC "test-topic"
#define TOPIC_ARN "arn:aws:sns:eu-central-1:000000000000:test-topic"
#define SUBSCRIPTION_ARN "arn:aws:sns:eu-central-1:000000000000:test-topic:b76548365bcd65"
#define QUEUE_URL "http://localhost:4567/000000000000/test-queue"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define OWNER "test-owner"

namespace AwsMock::Database {

    class SNSDatabaseTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        void TearDown() override {
            _snsDatabase.DeleteAllTopics();
            _snsDatabase.DeleteAllMessages();
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        SNSDatabase &_snsDatabase = SNSDatabase::instance();
    };

    TEST_F(SNSDatabaseTest, TopicCreateTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};

        // act
        const Entity::SNS::Topic result = _snsDatabase.CreateTopic(topic);

        // assert
        EXPECT_TRUE(result.topicName == TOPIC);
        EXPECT_TRUE(result.region == REGION);
        EXPECT_FALSE(result.oid.empty());
    }

    TEST_F(SNSDatabaseTest, TopicGetByIdTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const Entity::SNS::Topic result = _snsDatabase.GetTopicById(topic.oid);

        // assert
        EXPECT_TRUE(result.topicArn == topic.topicArn);
    }

    TEST_F(SNSDatabaseTest, TopicGetByArnTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const Entity::SNS::Topic result = _snsDatabase.GetTopicByArn(topic.topicArn);

        // assert
        EXPECT_TRUE(result.topicArn == topic.topicArn);
    }

    TEST_F(SNSDatabaseTest, TopicUpdateTest) {

        // arrange
        Entity::SNS::Topic topic{.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const std::string url = "http://localhost:4567/" + topic.topicName;
        topic.topicUrl = url;
        const Entity::SNS::Topic result = _snsDatabase.UpdateTopic(topic);

        // assert
        EXPECT_TRUE(result.topicUrl == topic.topicUrl);
    }

    TEST_F(SNSDatabaseTest, TopicCountTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const long result = _snsDatabase.CountTopics(topic.region);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SNSDatabaseTest, TopicListTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const Entity::SNS::TopicList result = _snsDatabase.ListTopics(topic.region);

        // assert
        EXPECT_EQ(result.size(), 1);
    }

    TEST_F(SNSDatabaseTest, TopicGetBySubscriptionArnTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        topic.subscriptions.push_back({.protocol = "sqs", .endpoint = QUEUE_URL, .subscriptionArn = SUBSCRIPTION_ARN});
        topic = _snsDatabase.UpdateTopic(topic);

        // act
        const Entity::SNS::TopicList result = _snsDatabase.GetTopicsBySubscriptionArn(SUBSCRIPTION_ARN);

        // assert
        EXPECT_EQ(1, result.size());
        EXPECT_TRUE(result[0].topicName == TOPIC);
    }

    TEST_F(SNSDatabaseTest, TopicDeleteTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        _snsDatabase.DeleteTopic(topic);
        bool result = _snsDatabase.TopicExists(topic.region, topic.topicName);

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(SNSDatabaseTest, MessageCreateTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY};

        // act
        Entity::SNS::Message result = _snsDatabase.CreateMessage(message);

        // assert
        EXPECT_FALSE(result.oid.empty());
        EXPECT_TRUE(result.message == BODY);
    }

    TEST_F(SNSDatabaseTest, MessageCountTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY};
        message = _snsDatabase.CreateMessage(message);

        // act
        long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SNSDatabaseTest, MessageDeleteTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY, .messageId = "abcd"};
        message = _snsDatabase.CreateMessage(message);

        // act
        _snsDatabase.DeleteMessage(message);
        long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        EXPECT_EQ(0, result);
    }

    TEST_F(SNSDatabaseTest, MessagesDeleteTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);

        std::vector<std::string> messageIds;
        for (int i = 0; i < 10; i++) {
            Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY, .messageId = "test" + std::to_string(i)};
            message = _snsDatabase.CreateMessage(message);
            messageIds.emplace_back(message.messageId);
        }

        // act
        _snsDatabase.DeleteMessages(_region, TOPIC_ARN, messageIds);
        long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        EXPECT_EQ(0, result);
    }

    TEST_F(SNSDatabaseTest, MessageDeleteAllTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY, .messageId = "abcd"};
        message = _snsDatabase.CreateMessage(message);

        // act
        _snsDatabase.DeleteAllMessages();
        long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        EXPECT_EQ(0, result);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_SNS_DATABASE_TEST_H