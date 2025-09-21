//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SNS_MEMORYDB_TEST_H
#define AWMOCK_CORE_SNS_MEMORYDB_TEST_H

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

    struct SNSMemoryDbTest {

        SNSMemoryDbTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~SNSMemoryDbTest() {
            long count = _snsDatabase.DeleteAllTopics();
            log_debug << "SNS topics deleted, count: " << count;
            count = _snsDatabase.DeleteAllMessages();
            log_debug << "SNS messages deleted, count: " << count;
        }

        std::string _region;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        SNSDatabase &_snsDatabase = SNSDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(TopicCreateMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};

        // act
        const Entity::SNS::Topic result = _snsDatabase.CreateTopic(topic);

        // assert
        BOOST_CHECK_EQUAL(result.topicName, TOPIC);
        BOOST_CHECK_EQUAL(result.region, REGION);
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(TopicGetByIdMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const Entity::SNS::Topic result = _snsDatabase.GetTopicById(topic.oid);

        // assert
        BOOST_CHECK_EQUAL(result.topicArn, topic.topicArn);
    }

    BOOST_FIXTURE_TEST_CASE(TopicGetByArnMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const Entity::SNS::Topic result = _snsDatabase.GetTopicByArn(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(result.topicArn, topic.topicArn);
    }

    BOOST_FIXTURE_TEST_CASE(TopicUpdateMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic{.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const std::string url = "http://localhost:4567/" + topic.topicName;
        topic.topicUrl = url;
        const Entity::SNS::Topic result = _snsDatabase.UpdateTopic(topic);

        // assert
        BOOST_CHECK_EQUAL(result.topicUrl, topic.topicUrl);
    }

    BOOST_FIXTURE_TEST_CASE(TopicCountMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const long result = _snsDatabase.CountTopics(topic.region);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(TopicListMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        const Entity::SNS::TopicList result = _snsDatabase.ListTopics(topic.region);

        // assert
        BOOST_CHECK_EQUAL(result.size(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(TopicGetBySubscriptionArnMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        topic.subscriptions.push_back({.protocol = "sqs", .endpoint = QUEUE_URL, .subscriptionArn = SUBSCRIPTION_ARN});
        topic = _snsDatabase.UpdateTopic(topic);

        // act
        const Entity::SNS::TopicList result = _snsDatabase.GetTopicsBySubscriptionArn(SUBSCRIPTION_ARN);

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
        BOOST_CHECK_EQUAL(result[0].topicName, TOPIC);
    }

    BOOST_FIXTURE_TEST_CASE(TopicDeleteMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);

        // act
        _snsDatabase.DeleteTopic(topic);
        const bool result = _snsDatabase.TopicExists(topic.region, topic.topicName);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(MessageCreateMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY};

        // act
        const Entity::SNS::Message result = _snsDatabase.CreateMessage(message);

        // assert
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(result.message, BODY);
    }

    BOOST_FIXTURE_TEST_CASE(MessageCountMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY};
        message = _snsDatabase.CreateMessage(message);

        // act
        const long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(MessageDeleteMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY, .messageId = "abcd"};
        message = _snsDatabase.CreateMessage(message);

        // act
        _snsDatabase.DeleteMessage(message);
        const long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(0, result);
    }

    BOOST_FIXTURE_TEST_CASE(MessagesDeleteMTest, SNSDatabaseTest) {

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
        const long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(0, result);
    }

    BOOST_FIXTURE_TEST_CASE(MessageDeleteAllMTest, SNSDatabaseTest) {

        // arrange
        Entity::SNS::Topic topic = {.region = _region, .topicName = TOPIC, .owner = OWNER, .topicArn = TOPIC_ARN};
        topic = _snsDatabase.CreateTopic(topic);
        Entity::SNS::Message message = {.region = _region, .topicArn = topic.topicArn, .message = BODY, .messageId = "abcd"};
        message = _snsDatabase.CreateMessage(message);

        // act
        const long count = _snsDatabase.DeleteAllMessages();
        const long result = _snsDatabase.CountMessages(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(0, result);
        BOOST_CHECK_EQUAL(1, count);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_SNS_MEMORYDB_TEST_H