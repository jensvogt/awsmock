//
// Created by vogje01 on 21/10/2023.
//

#ifndef AWMOCK_SQS_CLI_INTEGRATIONTEST_H
#define AWMOCK_SQS_CLI_INTEGRATIONTEST_H

// C++ includes
#include <string>

// GTest includes
#include <gtest/gtest.h>

// Boost includes
#include <boost/asio/thread_pool.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/dto/sqs/CreateQueueResponse.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/sqs/SQSServer.h>

// Test includes
#include "TestBase.h"
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define SQS_ACCOUNT_ID "000000000000"
#define TEST_QUEUE std::string("test-queue")

namespace AwsMock::Service {

    /**
     * @brief Test the SQS command line interface of AwsMock.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSServerCliTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // General configuration
            StartGateway();
            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {
            _sqsDatabase.DeleteAllMessages();
            _sqsDatabase.DeleteAllQueues();
        }

        static std::string GetReceiptHandle(const std::string &jsonString) {

            if (const value document = bsoncxx::from_json(jsonString); document.find("Messages") != document.end()) {
                std::string receiptHandle;
                for (const bsoncxx::array::view arrayView{document["Messages"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
                    receiptHandle = Core::Bson::BsonUtils::GetStringValue(element, "ReceiptHandle");
                }
                return receiptHandle;
            }
            return {};
        }

        std::string _endpoint, _queueUrl, _region;
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
    };

    TEST_F(SQSServerCliTest, QueueCreateTest) {

        // arrange

        // act
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();

        // assert
        EXPECT_EQ(1, queueList.size());
        EXPECT_TRUE(Core::StringUtils::Contains(output, TEST_QUEUE));
    }

    TEST_F(SQSServerCliTest, QueueListTest) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});

        // act
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "list-queues", "--endpoint", _endpoint});

        // assert
        EXPECT_TRUE(Core::StringUtils::Contains(output1, TEST_QUEUE));
    }

    TEST_F(SQSServerCliTest, QueueGetUrlTest) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});

        // act
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "get-queue-url", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});

        // assert
        EXPECT_TRUE(Core::StringUtils::Contains(output1, TEST_QUEUE));
    }

    TEST_F(SQSServerCliTest, QueuePurgeTest) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "purge-queue", "--queue-url", queueUrl, "--endpoint", _endpoint});
        const long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl));

        // assert
        EXPECT_EQ(0, messageCount);
    }

    TEST_F(SQSServerCliTest, QueueDeleteTest) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "get-queue-url", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "delete-queue", "--queue-url", queueUrl, "--endpoint", _endpoint});
        const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();

        // assert
        EXPECT_EQ(0, queueList.size());
    }

    TEST_F(SQSServerCliTest, MessageSendTest) {
        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        // act
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});
        const long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl));

        // assert
        EXPECT_EQ(1, messageCount);
    }

    TEST_F(SQSServerCliTest, MessageReceiveTest) {
        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "receive-message", "--queue-url", queueUrl, "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output3.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(output3, "Messages"));
    }

    TEST_F(SQSServerCliTest, MessageDeleteTest) {
        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});

        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "receive-message", "--queue-url", queueUrl, "--endpoint", _endpoint});
        const std::string receiptHandle = GetReceiptHandle(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "delete-message", "--queue-url", queueUrl, "--receipt-handle", receiptHandle, "--endpoint", _endpoint});
        const long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl));

        // assert
        EXPECT_EQ(0, messageCount);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SQS_CLI_INTEGRATIONTEST_H