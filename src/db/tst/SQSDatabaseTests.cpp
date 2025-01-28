//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_SQSDATABASETEST_H
#define AWMOCK_CORE_SQSDATABASETEST_H

// C++ includes
#include <chrono>

// GTest includes
#include <gtest/gtest.h>

// Boost includes
#include <boost/thread.hpp>

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

    class SQSDatabaseTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _queueArn = Core::CreateSQSQueueArn(QUEUE_NAME);
            _queueUrl = Core::CreateSQSQueueUrl(QUEUE_NAME);
            _dlqueueUrl = Core::CreateSQSQueueUrl(DLQ_NAME);
            _dlqueueArn = Core::CreateSQSQueueArn(DLQ_NAME);
            _region = _configuration.GetValueString("awsmock.region");
        }

        void TearDown() override {
            _sqsDatabase.DeleteAllQueues();
            _sqsDatabase.DeleteAllMessages();
        }

        std::string _region, _queueUrl, _queueArn, _dlqueueUrl, _dlqueueArn;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        SQSDatabase &_sqsDatabase = SQSDatabase::instance();
    };

    TEST_F(SQSDatabaseTest, QueueCreateTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};

        // act
        Entity::SQS::Queue result = _sqsDatabase.CreateQueue(queue);

        // assert
        EXPECT_TRUE(result.name == QUEUE_NAME);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(SQSDatabaseTest, QueueUrlExistsTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        _sqsDatabase.CreateQueue(queue);

        // act
        bool result = _sqsDatabase.QueueUrlExists(_region, _queueUrl);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(SQSDatabaseTest, QueueArnExistsTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        _sqsDatabase.CreateQueue(queue);

        // act
        bool result = _sqsDatabase.QueueArnExists(_queueArn);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(SQSDatabaseTest, QueueByIdTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        Entity::SQS::Queue result = _sqsDatabase.GetQueueById(queue.oid);

        // assert
        EXPECT_TRUE(result.name == QUEUE_NAME);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(SQSDatabaseTest, QueueByArnTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        Entity::SQS::Queue result = _sqsDatabase.GetQueueByArn(queue.queueArn);

        // assert
        EXPECT_TRUE(result.name == QUEUE_NAME);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(SQSDatabaseTest, QueueByUrlTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        Entity::SQS::Queue result = _sqsDatabase.GetQueueByUrl(_region, _queueUrl);

        // assert
        EXPECT_TRUE(result.name == QUEUE_NAME);
        EXPECT_TRUE(result.region == _region);
    }

    TEST_F(SQSDatabaseTest, QueueListTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        Entity::SQS::QueueList result = _sqsDatabase.ListQueues(queue.region);

        // assert
        EXPECT_EQ(result.size(), 1);
    }

    TEST_F(SQSDatabaseTest, QueueListPagingTest) {

        // arrange
        for (int i = 0; i < 10; i++) {
            std::string name = std::string(QUEUE_NAME) + "_" + std::to_string(i);
            Entity::SQS::Queue queue = {.region = _region, .name = name, .owner = OWNER, .queueUrl = _queueUrl};
            _sqsDatabase.CreateQueue(queue);
        }

        // act
        Entity::SQS::QueueList result = _sqsDatabase.ListQueues({}, 5, 0, {}, _region);

        // assert
        EXPECT_EQ(result.size(), 5);
        EXPECT_TRUE(result.front().name == std::string(QUEUE_NAME) + "_" + std::to_string(0));
    }

    TEST_F(SQSDatabaseTest, QueueListPagingNextTest) {

        // arrange
        for (int i = 0; i < 10; i++) {
            const std::string name = std::string(QUEUE_NAME) + "_" + std::to_string(i);
            Entity::SQS::Queue queue = {.region = _region, .name = name, .owner = OWNER, .queueUrl = _queueUrl};
            _sqsDatabase.CreateQueue(queue);
        }

        // act
        const Entity::SQS::QueueList result = _sqsDatabase.ListQueues({}, 5, 0, {}, _region);
        const Entity::SQS::QueueList result2 = _sqsDatabase.ListQueues({}, 5, 1, {}, _region);

        // assert
        EXPECT_EQ(result2.size(), 5);
        EXPECT_TRUE(result2.front().name == std::string(QUEUE_NAME) + "_" + std::to_string(5));
    }

    TEST_F(SQSDatabaseTest, QueuePurgeTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);

        // act
        _sqsDatabase.PurgeQueue(queue.queueArn);
        const long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        EXPECT_EQ(0, result);
    }

    TEST_F(SQSDatabaseTest, QueueUpdateTest) {

        // arrange
        const std::string _queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME);
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        queue.owner = "root";
        const Entity::SQS::Queue result = _sqsDatabase.UpdateQueue(queue);

        // assert
        EXPECT_TRUE(result.oid == queue.oid);
        EXPECT_TRUE(result.owner == queue.owner);
    }

    TEST_F(SQSDatabaseTest, QueueCountTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        _sqsDatabase.CreateQueue(queue);

        // act
        const long result = _sqsDatabase.CountQueues({}, _region);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SQSDatabaseTest, QueueCountTotalTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        _sqsDatabase.CreateQueue(queue);

        // act
        long result = _sqsDatabase.CountQueues();

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SQSDatabaseTest, QueueDeleteTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        queue = _sqsDatabase.CreateQueue(queue);

        // act
        _sqsDatabase.DeleteQueue(queue);
        bool result = _sqsDatabase.QueueExists(queue.region, queue.queueUrl);

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(SQSDatabaseTest, MessageCreateTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};

        // act
        Entity::SQS::Message result = _sqsDatabase.CreateMessage(message);

        // assert
        EXPECT_FALSE(result.oid.empty());
        EXPECT_TRUE(result.body == BODY);
    }

    TEST_F(SQSDatabaseTest, MessageExistsTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY, .messageId = "10bdf54e6f7"};
        message = _sqsDatabase.CreateMessage(message);

        // act
        bool result = _sqsDatabase.MessageExists(message.receiptHandle);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(SQSDatabaseTest, MessageGetByIdTest) {

        // arrange
        Entity::SQS::Queue queue = {.region = _region, .name = QUEUE_NAME, .owner = OWNER, .queueUrl = _queueUrl};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        message = _sqsDatabase.CreateMessage(message);

        // act
        Entity::SQS::Message result = _sqsDatabase.GetMessageById(message.oid);

        // assert
        EXPECT_TRUE(message.oid == result.oid);
    }

    TEST_F(SQSDatabaseTest, MessageReceiveTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);

        // act
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 30, 3, "", -1, messageList);

        // assert
        EXPECT_FALSE(messageList.empty());
    }

    TEST_F(SQSDatabaseTest, MessageCountTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME)};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);

        // act
        long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SQSDatabaseTest, MessageCountStatusTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME)};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);

        // act
        long result = _sqsDatabase.CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SQSDatabaseTest, MessageResetTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 1, 1, "", -1, messageList);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // act
        _sqsDatabase.ResetMessages(queue.queueArn, 1);
        long result = _sqsDatabase.CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);

        // assert
        EXPECT_EQ(1, result);
    }

    TEST_F(SQSDatabaseTest, MessageDelayTest) {

        // arrange
        Entity::SQS::QueueAttribute queueAttribute;
        queueAttribute.delaySeconds = 1;
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = _queueArn, .attributes = queueAttribute};
        queue = _sqsDatabase.CreateQueue(queue);

        system_clock::time_point reset = system_clock::now();
        reset += std::chrono::seconds(queueAttribute.delaySeconds);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY, .status = Entity::SQS::MessageStatus::DELAYED, .reset = reset};
        _sqsDatabase.CreateMessage(message);

        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 1, 1, "", 3, messageList);
        EXPECT_EQ(0, messageList.size());
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // act
        const long delayed = _sqsDatabase.ResetDelayedMessages(queue.queueArn, queueAttribute.delaySeconds);
        const long result = _sqsDatabase.CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);

        // assert
        EXPECT_EQ(1, result);
        EXPECT_TRUE(delayed == 0);
    }

    TEST_F(SQSDatabaseTest, MessageGetByReceiptHandleTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME)};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 30, 3, "", -1, messageList);

        // act
        Entity::SQS::Message result = messageList[0];
        result = _sqsDatabase.GetMessageByReceiptHandle(result.receiptHandle);

        // assert
        EXPECT_EQ(result.receiptHandle, messageList[0].receiptHandle);
    }

    TEST_F(SQSDatabaseTest, MessageUpdateTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME)};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY, .receiptHandle = Core::AwsUtils::CreateSqsReceiptHandler()};
        message = _sqsDatabase.CreateMessage(message);

        // act
        message.status = Entity::SQS::MessageStatus::DELAYED;
        const Entity::SQS::Message result = _sqsDatabase.UpdateMessage(message);

        // assert
        EXPECT_TRUE(result.oid == message.oid);
        EXPECT_TRUE(result.status == Entity::SQS::MessageStatus::DELAYED);
    }

    TEST_F(SQSDatabaseTest, MessageDeleteTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME)};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);
        Entity::SQS::MessageList messageList;
        _sqsDatabase.ReceiveMessages(queue.queueArn, 30, 3, "", -1, messageList);

        // act
        const Entity::SQS::Message resultMessage = messageList[0];
        const long deleted = _sqsDatabase.DeleteMessage(resultMessage);
        const long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        EXPECT_EQ(0, result);
        EXPECT_TRUE(deleted > 0);
    }

    TEST_F(SQSDatabaseTest, MessageDeleteQueueTest) {

        // arrange
        Entity::SQS::Queue queue = {.owner = OWNER, .queueUrl = _queueUrl, .queueArn = Core::AwsUtils::CreateSQSQueueArn(_region, ACCOUNT_ID, QUEUE_NAME)};
        queue = _sqsDatabase.CreateQueue(queue);
        Entity::SQS::Message message = {.queueArn = queue.queueArn, .body = BODY};
        _sqsDatabase.CreateMessage(message);

        // act
        const long deleted = _sqsDatabase.DeleteMessages(queue.queueArn);
        const long result = _sqsDatabase.CountMessages(queue.queueArn);

        // assert
        EXPECT_EQ(0, result);
        EXPECT_TRUE(deleted > 0);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_SQSDATABASETEST_H