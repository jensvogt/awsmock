//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SQS_MEMORYDB_TEST_H
#define AWMOCK_CORE_SQS_MEMORYDB_TEST_H

// C++ includes
#include <chrono>
#include <thread>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SQSDatabase.h>

#define BODY "{\"TestObject\": \"TestValue\"}"
#define OWNER "test-owner"
#define QUEUE_NAME "test-queue"
#define DLQ_NAME "test-dlqueue"
#define ACCOUNT_ID "000000000000"

namespace AwsMock::Database {

    using std::chrono::system_clock;

    struct SQSMemoryDbTest {

        SQSMemoryDbTest() {
            _queueArn = Core::CreateSQSQueueArn(QUEUE_NAME);
            _queueUrl = Core::CreateSQSQueueUrl(QUEUE_NAME);
            _dlqueueUrl = Core::CreateSQSQueueUrl(DLQ_NAME);
            _dlqueueArn = Core::CreateSQSQueueArn(DLQ_NAME);
            _region = _configuration.GetValue<std::string>("awsmock.region");
        }

        ~SQSMemoryDbTest() {
            long count = _sqsDatabase.DeleteAllQueues();
            log_debug << "Deleting all queues, count: " << count;
            count = _sqsDatabase.DeleteAllMessages();
            log_debug << "Deleting all messages, count: " << count;
        }

        std::string _region, _queueUrl, _queueArn, _dlqueueUrl, _dlqueueArn;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        SQSDatabase &_sqsDatabase = SQSDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(QueueCreateMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;

        // act
        const Entity::SQS::Queue result = _sqsDatabase.CreateQueue(queue);

        // assert
        BOOST_CHECK_EQUAL(result.name, QUEUE_NAME);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(QueueUrlExistsMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        _sqsDatabase.CreateQueue(queue);

        // act
        const bool result = _sqsDatabase.QueueUrlExists(_region, _queueUrl);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(QueueArnExistsMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        _sqsDatabase.CreateQueue(queue);

        // act
        const bool result = _sqsDatabase.QueueArnExists(_queueArn);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(QueueByIdMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        const Entity::SQS::Queue result = _sqsDatabase.GetQueueById(queue.oid);

        // assert
        BOOST_CHECK_EQUAL(result.name, QUEUE_NAME);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(QueueByArnMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        const Entity::SQS::Queue result = _sqsDatabase.GetQueueByArn(queue.queueArn);

        // assert
        BOOST_CHECK_EQUAL(result.name, QUEUE_NAME);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(QueueByUrlMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        const Entity::SQS::Queue result = _sqsDatabase.GetQueueByUrl(_region, _queueUrl);

        // assert
        BOOST_CHECK_EQUAL(result.name, QUEUE_NAME);
        BOOST_CHECK_EQUAL(result.region, _region);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        const Entity::SQS::QueueList result = _sqsDatabase.ListQueues(queue.region);

        // assert
        BOOST_CHECK_EQUAL(result.size(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListPagingMTest, SQSMemoryDbTest) {

        // arrange
        for (int i = 0; i < 10; i++) {
            const std::string name = std::string(QUEUE_NAME) + "_" + std::to_string(i);
            Entity::SQS::Queue queue;
            queue.region = _region;
            queue.name = name;
            queue.owner = OWNER;
            queue.queueUrl = _queueUrl;
            _sqsDatabase.CreateQueue(queue);
        }

        // act
        const Entity::SQS::QueueList result = _sqsDatabase.ListQueues({}, 5, 0, {}, _region);

        // assert
        BOOST_CHECK_EQUAL(result.size(), 5);
        BOOST_CHECK_EQUAL(result.front().name, std::string(QUEUE_NAME) + "_" + std::to_string(0));
    }

    BOOST_FIXTURE_TEST_CASE(QueueListPagingNextMTest, SQSMemoryDbTest) {

        // arrange
        for (int i = 0; i < 10; i++) {
            const std::string name = std::string(QUEUE_NAME) + "_" + std::to_string(i);
            Entity::SQS::Queue queue;
            queue.region = _region;
            queue.name = name;
            queue.owner = OWNER;
            queue.queueUrl = _queueUrl;
            _sqsDatabase.CreateQueue(queue);
        }

        // act
        const Entity::SQS::QueueList result = _sqsDatabase.ListQueues({}, 5, 0, {}, _region);
        const Entity::SQS::QueueList result2 = _sqsDatabase.ListQueues({}, 5, 1, {}, _region);

        // assert
        BOOST_CHECK_EQUAL(result2.size(), 5);
        BOOST_CHECK_EQUAL(result2.front().name, std::string(QUEUE_NAME) + "_" + std::to_string(5));
    }

    BOOST_FIXTURE_TEST_CASE(QueuePurgeMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);

        // act
        const long count = _sqsDatabase.PurgeQueue(queue.queueArn);
        BOOST_CHECK_EQUAL(count > 0, true);
        const long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        BOOST_CHECK_EQUAL(0, result);
    }

    BOOST_FIXTURE_TEST_CASE(QueueUpdateMTest, SQSMemoryDbTest) {

        // arrange
        const std::string _queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        queue.owner = "root";
        const Entity::SQS::Queue result = _sqsDatabase.UpdateQueue(queue);

        // assert
        BOOST_CHECK_EQUAL(result.oid, queue.oid);
        BOOST_CHECK_EQUAL(result.owner, queue.owner);
    }

    BOOST_FIXTURE_TEST_CASE(QueueCountMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        _sqsDatabase.CreateQueue(queue);

        // act
        const long result = _sqsDatabase.CountQueues({}, _region);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(QueueCountTotalMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        _sqsDatabase.CreateQueue(queue);

        // act
        const long result = _sqsDatabase.CountQueues();

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(QueueDeleteMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        const long count = _sqsDatabase.DeleteQueue(queue);
        BOOST_CHECK_EQUAL(count > 0, true);
        const bool result = _sqsDatabase.QueueExists(queue.region, queue.queueUrl);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(SQSMessageCreateMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;

        // act
        const Entity::SQS::Message result = _sqsDatabase.CreateMessage(message);

        // assert
        BOOST_CHECK_EQUAL(result.oid.empty(), false);
        BOOST_CHECK_EQUAL(result.body, BODY);
    }

    BOOST_FIXTURE_TEST_CASE(MessageExistsMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        message.messageId = "10bdf54e6f7";
        message = _sqsDatabase.CreateMessage(message);

        // act
        const bool result = _sqsDatabase.MessageExists(message.receiptHandle);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(MessageGetByIdMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.region = _region;
        queue.name = QUEUE_NAME;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        message = _sqsDatabase.CreateMessage(message);

        // act
        const Entity::SQS::Message result = _sqsDatabase.GetMessageById(message.oid);

        // assert
        BOOST_CHECK_EQUAL(message.oid, result.oid);
    }

    BOOST_FIXTURE_TEST_CASE(MessageReceiveMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);

        // act
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 30, 3, "", -1, messageList);

        // assert
        BOOST_CHECK_EQUAL(messageList.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SQSMessageCountMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);

        // act
        const long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(MessageCountStatusMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);

        // act
        const long result = _sqsDatabase.CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(MessageResetMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 1, 1, "", -1, messageList);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // act
        const long count = _sqsDatabase.ResetMessages(queue.queueArn, 1);
        BOOST_CHECK_EQUAL(count > 0, true);
        const long result = _sqsDatabase.CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);

        // assert
        BOOST_CHECK_EQUAL(1, result);
    }

    BOOST_FIXTURE_TEST_CASE(MessageDelayMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::QueueAttribute queueAttribute;
        queueAttribute.delaySeconds = 1;
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = _queueArn;
        queue.attributes = queueAttribute;
        queue = _sqsDatabase.CreateQueue(queue);

        system_clock::time_point reset = system_clock::now();
        reset += std::chrono::seconds(queueAttribute.delaySeconds);
        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        message.status = Entity::SQS::MessageStatus::DELAYED;
        message.reset = reset;
        _sqsDatabase.CreateMessage(message);

        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 1, 1, "", 3, messageList);
        BOOST_CHECK_EQUAL(0, messageList.size());
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // act
        const long delayed = _sqsDatabase.ResetDelayedMessages(queue.queueArn, queueAttribute.delaySeconds);
        const long result = _sqsDatabase.CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);

        // assert
        BOOST_CHECK_EQUAL(1, result);
        BOOST_CHECK_EQUAL(0, delayed);
    }

    BOOST_FIXTURE_TEST_CASE(MessageGetByReceiptHandleMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 30, 3, "", -1, messageList);

        // act
        Entity::SQS::Message result = messageList[0];
        result = _sqsDatabase.GetMessageByReceiptHandle(result.receiptHandle);

        // assert
        BOOST_CHECK_EQUAL(result.receiptHandle, messageList[0].receiptHandle);
    }

    BOOST_FIXTURE_TEST_CASE(MessageUpdateMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        message.receiptHandle = Core::AwsUtils::CreateSqsReceiptHandler();
        message = _sqsDatabase.CreateMessage(message);

        // act
        message.status = Entity::SQS::MessageStatus::DELAYED;
        const Entity::SQS::Message result = _sqsDatabase.UpdateMessage(message);

        // assert
        BOOST_CHECK_EQUAL(result.oid, message.oid);
        BOOST_CHECK_EQUAL(result.status == Entity::SQS::MessageStatus::DELAYED, true);
    }

    BOOST_FIXTURE_TEST_CASE(SQSMessageDeleteMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 30, 3, "", -1, messageList);

        // act
        const Entity::SQS::Message resultMessage = messageList[0];
        const long deleted = _sqsDatabase.DeleteMessage(resultMessage);
        const long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        BOOST_CHECK_EQUAL(0, result);
        BOOST_CHECK_EQUAL(deleted > 0, true);
    }

    BOOST_FIXTURE_TEST_CASE(MessageDeleteQueueMTest, SQSMemoryDbTest) {

        // arrange
        Entity::SQS::Queue queue;
        queue.owner = OWNER;
        queue.queueUrl = _queueUrl;
        queue.queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        queue = _sqsDatabase.CreateQueue(queue);

        Entity::SQS::Message message;
        message.queueArn = queue.queueArn;
        message.body = BODY;
        _sqsDatabase.CreateMessage(message);

        // act
        const long deleted = _sqsDatabase.DeleteMessages(queue.queueArn);
        const long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        BOOST_CHECK_EQUAL(0, result);
        BOOST_CHECK_EQUAL(deleted > 0, true);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_SQS_MEMORYDB_TEST_H