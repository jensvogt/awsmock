
//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/SNSDatabase.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_TOPIC_NAME "test-topic"
#define TEST_MESSAGE_BODY "{\"testAttribute\":\"testValue\"}"

namespace AwsMock::Database {

    Entity::SNS::Message CreateDefaultSQSMessage(const std::string &topicArn) {
        Entity::SNS::Message message;
        message.topicArn = topicArn;
        message.message = TEST_MESSAGE_BODY;
        message.messageId = Core::AwsUtils::CreateMessageId();
        message.status = Entity::SNS::MessageStatus::INITIAL;
        return message;
    }

    Entity::SNS::Topic CreateDefaultQueue(const std::string &region, const std::string &topicName) {
        Entity::SNS::Topic topic;
        topic.region = region;
        topic.topicArn = Core::AwsUtils::CreateSNSTopicArn(region, TEST_ACCOUNT_ID, topicName);
        return topic;
    }

    struct SnsMemoryDbFixture {
        SnsMemoryDbFixture() = default;
        ~SnsMemoryDbFixture() {
            const long messageCount = SNSDatabase::instance().DeleteAllMessages();
            log_debug << "Messages deleted " << messageCount;
            const long topicCount = SNSDatabase::instance().DeleteAllTopics();
            log_debug << "Topics deleted " << topicCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SnsMemoryDbTests, SnsMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(CreateTopicTest) {

        // arrange
        const SNSDatabase &snsDatabase = SNSDatabase::instance();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);

        // act
        topic = snsDatabase.CreateTopic(topic);

        // assert
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        BOOST_CHECK_EQUAL(0, topic.messages);
        BOOST_CHECK_EQUAL(0, topic.messagesSend);
        BOOST_CHECK_EQUAL(0, topic.messagesResend);
    }

    BOOST_AUTO_TEST_CASE(CreateMessageTest) {

        // arrange
        const SNSDatabase &snsDatabase = SNSDatabase::instance();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);
        topic = snsDatabase.CreateTopic(topic);
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        Entity::SNS::Message message = CreateDefaultSQSMessage(topic.topicArn);

        // act
        message = snsDatabase.CreateMessage(message);
        snsDatabase.AdjustMessageCounters();
        topic = snsDatabase.GetTopicByArn(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(false, message.messageId.empty());
        BOOST_CHECK_EQUAL(1, topic.messages);
    }

    BOOST_AUTO_TEST_CASE(DeleteMessageTest) {

        // arrange
        const SNSDatabase &snsDatabase = SNSDatabase::instance();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);
        topic = snsDatabase.CreateTopic(topic);
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        Entity::SNS::Message message = CreateDefaultSQSMessage(topic.topicArn);
        message = snsDatabase.CreateMessage(message);
        BOOST_CHECK_EQUAL(false, message.messageId.empty());

        // act
        const long count = snsDatabase.DeleteMessage(message);
        snsDatabase.AdjustMessageCounters();
        topic = snsDatabase.GetTopicByArn(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(0, topic.messages);
    }

    BOOST_AUTO_TEST_CASE(DeleteAllMessagesTest) {

        // arrange
        const SNSDatabase &snsDatabase = SNSDatabase::instance();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);
        topic = snsDatabase.CreateTopic(topic);
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        Entity::SNS::Message message = CreateDefaultSQSMessage(topic.topicArn);
        message = snsDatabase.CreateMessage(message);
        BOOST_CHECK_EQUAL(false, message.messageId.empty());

        // act
        const long count = snsDatabase.DeleteAllMessages();
        snsDatabase.AdjustMessageCounters();
        topic = snsDatabase.GetTopicByArn(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(0, topic.messages);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database