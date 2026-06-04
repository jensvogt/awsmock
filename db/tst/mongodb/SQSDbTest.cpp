//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/sqs/ISQSRepository.h>

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
        SqsDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB);
        }
        ~SqsDbFixture() {
            const long messageCount = RepositoryFactory::instance().sqsRepository()->deleteAllMessages();
            log_debug << "Messages deleted " << messageCount;
            const long queueCount = RepositoryFactory::instance().sqsRepository()->deleteAllQueues();
            log_debug << "Queues deleted " << queueCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SqsDbTests, SqsDbFixture)

    BOOST_AUTO_TEST_CASE(createQueueTest) {

        // arrange
        const std::shared_ptr<ISQSRepository> sqsRepository = RepositoryFactory::instance().sqsRepository();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);

        // act
        queue = sqsRepository->createQueue(queue);

        // assert
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessages);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessagesNotVisible);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessagesDelayed);
    }

    BOOST_AUTO_TEST_CASE(deleteQueueTest) {

        // arrange
        const std::shared_ptr<ISQSRepository> sqsRepository = RepositoryFactory::instance().sqsRepository();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue = sqsRepository->createQueue(queue);
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());

        // act
        const long result = sqsRepository->deleteQueue(queue);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_AUTO_TEST_CASE(DeleteAllQueueTest) {

        // arrange
        const std::shared_ptr<ISQSRepository> sqsRepository = RepositoryFactory::instance().sqsRepository();
        Entity::SQS::Queue queue1 = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue1 = sqsRepository->createQueue(queue1);
        BOOST_CHECK_EQUAL(false, queue1.arn.empty());
        BOOST_CHECK_EQUAL(false, queue1.oid.empty());
        BOOST_CHECK_EQUAL(false, queue1.name.empty());
        Entity::SQS::Queue queue2 = CreateDefaultTopic(TEST_REGION, std::string(TEST_QUEUE_NAME) + "_1");
        queue2 = sqsRepository->createQueue(queue2);
        BOOST_CHECK_EQUAL(false, queue2.arn.empty());
        BOOST_CHECK_EQUAL(false, queue2.oid.empty());
        BOOST_CHECK_EQUAL(false, queue2.name.empty());

        // act
        const long result = sqsRepository->deleteAllQueues();

        // assert
        BOOST_CHECK_EQUAL(2, result);
    }

    BOOST_AUTO_TEST_CASE(SendMessageTest) {

        // arrange
        const std::shared_ptr<ISQSRepository> sqsRepository = RepositoryFactory::instance().sqsRepository();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue = sqsRepository->createQueue(queue);
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        Entity::SQS::Message message = CreateDefaultSNSMessage(TEST_REGION, queue.name);

        // act
        message = sqsRepository->createMessage(message);
        sqsRepository->adjustMessageCounters();
        queue = sqsRepository->getQueueByArn(queue.arn);

        // assert
        BOOST_CHECK_EQUAL(true, message.receiptHandle.empty());
        BOOST_CHECK_EQUAL(false, message.messageId.empty());
        BOOST_CHECK_EQUAL(1, queue.attributes.approximateNumberOfMessages);
    }

    BOOST_AUTO_TEST_CASE(DeleteMessageTest) {

        // arrange
        const std::shared_ptr<ISQSRepository> sqsRepository = RepositoryFactory::instance().sqsRepository();
        Entity::SQS::Queue queue = CreateDefaultTopic(TEST_REGION, TEST_QUEUE_NAME);
        queue = sqsRepository->createQueue(queue);
        BOOST_CHECK_EQUAL(false, queue.arn.empty());
        BOOST_CHECK_EQUAL(false, queue.oid.empty());
        BOOST_CHECK_EQUAL(false, queue.name.empty());
        Entity::SQS::Message message = CreateDefaultSNSMessage(TEST_REGION, queue.name);
        message = sqsRepository->createMessage(message);
        BOOST_CHECK_EQUAL(true, message.receiptHandle.empty());
        BOOST_CHECK_EQUAL(false, message.messageId.empty());

        // act
        const long count = sqsRepository->deleteMessage(message);
        sqsRepository->adjustMessageCounters();
        queue = sqsRepository->getQueueByArn(queue.arn);

        // assert
        BOOST_CHECK_EQUAL(1, count);
        BOOST_CHECK_EQUAL(0, queue.attributes.approximateNumberOfMessages);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
