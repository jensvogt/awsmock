//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <../../include/awsmock/repository/sqs/SQSDatabase.h>
#include <awsmock/core/AwsUtils.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_QUEUE_NAME "test-queue"
#define TEST_MESSAGE_BODY "{\"testAttribute\":\"testValue\"}"

namespace Awsmock::Database {

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
        queue.arn = Core::AwsUtils::CreateSQSQueueArn(region, TEST_ACCOUNT_ID, queueName);
        queue.url = Core::AwsUtils::ConvertSQSQueueArnToUrl(queue.arn);
        return queue;
    }

    struct SqsDbFixture {
        SqsDbFixture() = default;
        ~SqsDbFixture() {
            const long messageCount = SQSDatabase::instance().DeleteAllMessages();
            log_debug << "Messages deleted " << messageCount;
            const long queueCount = SQSDatabase::instance().DeleteAllQueues();
            log_debug << "Queues deleted " << queueCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SqsDbTests, SqsDbFixture)

    BOOST_AUTO_TEST_CASE(CreateQueueTest) {

        // arrange
        const SQSDatabase &sqsDatabase = SQSDatabase::instance();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);

        // act
        queue = sqsDatabase.CreateQueue(queue);

        // assert
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessages);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessagesNotVisible);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessagesDelayed);
    }

    BOOST_AUTO_TEST_CASE(DeleteQueueTest) {

        // arrange
        const SQSDatabase &sqsDatabase = SQSDatabase::instance();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue = sqsDatabase.CreateQueue(queue);
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());

        // act
        const long result = sqsDatabase.DeleteQueue(queue);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(DeleteAllQueueTest) {

        // arrange
        const SQSDatabase &sqsDatabase = SQSDatabase::instance();
        Entity::SQS::Queue queue1 = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue1 = sqsDatabase.CreateQueue(queue1);
        BOOST_CHECK_EQUAL(false, queue1.arn.empty());
        BOOST_CHECK_EQUAL(false, queue1.oid.empty());
        BOOST_CHECK_EQUAL(false, queue1.name.empty());
        Entity::SQS::Queue queue2 = CreateDefaultTopic(TEST_REGION, std::string(TEST_QUEUE_NAME) + "_1");
        queue2 = sqsDatabase.CreateQueue(queue2);
        BOOST_CHECK_EQUAL(false, queue2.arn.empty());
        BOOST_CHECK_EQUAL(false, queue2.oid.empty());
        BOOST_CHECK_EQUAL(false, queue2.name.empty());

        // act
        const long result = sqsDatabase.DeleteAllQueues();

        // assert
        BOOST_CHECK_EQUAL(2, result);
    }

    BOOST_AUTO_TEST_CASE(SendMessageTest) {

        // arrange
        const SQSDatabase &sqsDatabase = SQSDatabase::instance();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue = sqsDatabase.CreateQueue(queue);
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        Entity::SQS::Message message = CreateDefaultSNSMessage(TEST_REGION, queue.name);

        // act
        message = sqsDatabase.CreateMessage(message);
        sqsDatabase.AdjustMessageCounters();
        queue = sqsDatabase.GetQueueByArn(queue.arn);

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
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        Entity::SQS::Message message = CreateDefaultSNSMessage(TEST_REGION, queue.name);
        message = sqsDatabase.CreateMessage(message);
        BOOST_CHECK_EQUAL(true, message.receiptHandle.empty());
        BOOST_CHECK_EQUAL(false, message.messageId.empty());

        // act
        const long count = sqsDatabase.DeleteMessage(message);
        sqsDatabase.AdjustMessageCounters();
        queue = sqsDatabase.GetQueueByArn(queue.arn);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessages);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
