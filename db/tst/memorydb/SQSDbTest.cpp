//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/SQSDatabase.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_QUEUE_NAME "test-queue"
#define TEST_MESSAGE_BODY "{\"testAttribute\":\"testValue\"}"

namespace AwsMock::Database {

    Entity::SQS::Message CreateDefaultSNSMessage(const std::string &region, const std::string &queueName) {
        Entity::SQS::Message message;
        message.queueArn = Core::AwsUtils::CreateSQSQueueArn(region, TEST_ACCOUNT_ID, queueName);
        message.queueName = queueName;
        message.body = TEST_MESSAGE_BODY;
        message.messageId = Core::AwsUtils::CreateMessageId();
        return message;
    }

    Entity::SQS::Queue CreateDefaultTopic(const std::string &region, const std::string &queueName) {
        Entity::SQS::Queue queue;
        queue.region = region;
        queue.name = queueName;
        queue.queueArn = Core::AwsUtils::CreateSQSQueueArn(region, TEST_ACCOUNT_ID, queueName);
        queue.queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(queue.queueArn);
        return queue;
    }

    struct SqsMemoryDbFixture {
        SqsMemoryDbFixture() = default;
        ~SqsMemoryDbFixture() {
            const long messageCount = SQSDatabase::instance().DeleteAllMessages();
            log_debug << "Messages deleted " << messageCount;
            const long queueCount = SQSDatabase::instance().DeleteAllQueues();
            log_debug << "Queues deleted " << queueCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SqsMemoryDbTests, SqsMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(CreateQueueTest) {

        // arrange
        const SQSDatabase &sqsDatabase = SQSDatabase::instance();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);

        // act
        queue = sqsDatabase.CreateQueue(queue);

        // assert
        BOOST_CHECK_EQUAL(false, queue.queueArn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessages);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessagesNotVisible);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessagesDelayed);
    }

    BOOST_AUTO_TEST_CASE(SendMessageTest) {

        // arrange
        const SQSDatabase &sqsDatabase = SQSDatabase::instance();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue = sqsDatabase.CreateQueue(queue);
        BOOST_CHECK_EQUAL(false, queue.queueArn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        Entity::SQS::Message message = CreateDefaultSNSMessage(TEST_REGION, queue.name);

        // act
        message = sqsDatabase.CreateMessage(message);
        sqsDatabase.AdjustMessageCounters();
        queue = sqsDatabase.GetQueueByArn(queue.queueArn);

        // assert
        BOOST_CHECK_EQUAL(true, message.receiptHandle.empty());
        BOOST_CHECK_EQUAL(false, message.messageId.empty());
        BOOST_CHECK_EQUAL(1, queue.attributes.approximateNumberOfMessages);
    }

    BOOST_AUTO_TEST_CASE(DeleteMessageTest) {

        // arrange
        const SQSDatabase &sqsDatabase = SQSDatabase::instance();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue = sqsDatabase.CreateQueue(queue);
        BOOST_CHECK_EQUAL(false, queue.queueArn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        Entity::SQS::Message message = CreateDefaultSNSMessage(TEST_REGION, queue.name);
        message = sqsDatabase.CreateMessage(message);
        BOOST_CHECK_EQUAL(true, message.receiptHandle.empty());
        BOOST_CHECK_EQUAL(false, message.messageId.empty());

        // act
        const long count = sqsDatabase.DeleteMessage(message);
        sqsDatabase.AdjustMessageCounters();
        queue = sqsDatabase.GetQueueByArn(queue.queueArn);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessages);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database
