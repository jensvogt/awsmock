
//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/sns/ISNSRepository.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_TOPIC_NAME "test-topic"
#define TEST_MESSAGE_BODY "{\"testAttribute\":\"testValue\"}"

namespace Awsmock::Database {

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

    struct SnsDbFixture {
        SnsDbFixture() = default;
        ~SnsDbFixture() {
            const long messageCount = RepositoryFactory::instance().snsRepository()->deleteAllMessages();
            log_debug << "Messages deleted " << messageCount;
            const long topicCount = RepositoryFactory::instance().snsRepository()->deleteAllTopics();
            log_debug << "Topics deleted " << topicCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SnsDbTests, SnsDbFixture)

    BOOST_AUTO_TEST_CASE(createTopicTest) {

        // arrange
        const std::shared_ptr<ISNSRepository> snsRepository = RepositoryFactory::instance().snsRepository();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);

        // act
        topic = snsRepository->createTopic(topic);

        // assert
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        BOOST_CHECK_EQUAL(0, topic.messages);
        BOOST_CHECK_EQUAL(0, topic.messagesSend);
        BOOST_CHECK_EQUAL(0, topic.messagesResend);
    }

    BOOST_AUTO_TEST_CASE(createMessageTest) {

        // arrange
        const std::shared_ptr<ISNSRepository> snsRepository = RepositoryFactory::instance().snsRepository();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);
        topic = snsRepository->createTopic(topic);
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        Entity::SNS::Message message = CreateDefaultSQSMessage(topic.topicArn);

        // act
        message = snsRepository->createMessage(message);
        snsRepository->adjustMessageCounters();
        topic = snsRepository->getTopicByArn(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(false, message.messageId.empty());
        BOOST_CHECK_EQUAL(1, topic.messages);
    }

    BOOST_AUTO_TEST_CASE(deleteMessageTest) {

        // arrange
        const std::shared_ptr<ISNSRepository> snsRepository = RepositoryFactory::instance().snsRepository();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);
        topic = snsRepository->createTopic(topic);
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        Entity::SNS::Message message = CreateDefaultSQSMessage(topic.topicArn);
        message = snsRepository->createMessage(message);
        BOOST_CHECK_EQUAL(false, message.messageId.empty());

        // act
        const long count = snsRepository->deleteMessage(message);
        snsRepository->adjustMessageCounters();
        topic = snsRepository->getTopicByArn(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(0, topic.messages);
    }

    BOOST_AUTO_TEST_CASE(DeleteAllMessagesTest) {

        // arrange
        const std::shared_ptr<ISNSRepository> snsRepository = RepositoryFactory::instance().snsRepository();
        Entity::SNS::Topic topic = CreateDefaultQueue(TEST_REGION, TEST_TOPIC_NAME);
        topic = snsRepository->createTopic(topic);
        BOOST_CHECK_EQUAL(false, topic.topicArn.empty());
        BOOST_CHECK_EQUAL(false, topic.oid.empty());
        Entity::SNS::Message message = CreateDefaultSQSMessage(topic.topicArn);
        message = snsRepository->createMessage(message);
        BOOST_CHECK_EQUAL(false, message.messageId.empty());

        // act
        const long count = snsRepository->deleteAllMessages();
        snsRepository->adjustMessageCounters();
        topic = snsRepository->getTopicByArn(topic.topicArn);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(0, topic.messages);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database