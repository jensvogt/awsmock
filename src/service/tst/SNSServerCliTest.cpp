//
// Created by vogje01 on 21/10/2023.
//

#ifndef AWMOCK_SERVICE_SNS_CLI_INTEGRATION_TEST_H
#define AWMOCK_SERVICE_SNS_CLI_INTEGRATION_TEST_H

// C++ includes
#include <string>

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/dto/sqs/CreateQueueResponse.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/sns/SNSServer.h>
#include <awsmock/service/sqs/SQSServer.h>

#define TEST_TOPIC std::string("test-topic")

namespace AwsMock::Service {

    /**
     * @brief Test the SNS command line interface of AwsMock.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSServerCliTest : public ::testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // General configuration
            StartGateway();
            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {
            _snsDatabase.DeleteAllMessages();
            _snsDatabase.DeleteAllTopics();
            _sqsDatabase.DeleteAllMessages();
            _sqsDatabase.DeleteAllQueues();
        }

        static std::string GetTopicArn(const std::string &jsonString) {

            if (const value document = bsoncxx::from_json(jsonString); document.find("Topics") != document.end()) {
                std::string topicArn;
                for (const bsoncxx::array::view arrayView{document["Topics"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
                    topicArn = Core::Bson::BsonUtils::GetStringValue(element, "TopicArn");
                }
                return topicArn;
            }
            return {};
        }

        static std::string GetMessageId(const std::string &jsonString) {

            if (const value document = bsoncxx::from_json(jsonString); document.find("MessageId") != document.end()) {
                std::string messageId = Core::Bson::BsonUtils::GetStringValue(document, "MessageId");
                return messageId;
            }
            return {};
        }

        static std::string GetQueueUrl(const std::string &jsonString) {

            if (const value document = bsoncxx::from_json(jsonString); document.find("QueueUrl") != document.end()) {
                std::string queueUrl = Core::Bson::BsonUtils::GetStringValue(document, "QueueUrl");
                return queueUrl;
            }
            return {};
        }

        static std::string GetSubscriptionArn(const std::string &jsonString) {

            if (const value document = bsoncxx::from_json(jsonString); document.find("SubscriptionArn") != document.end()) {
                std::string subscriptionArn = Core::Bson::BsonUtils::GetStringValue(document, "SubscriptionArn");
                return subscriptionArn;
            }
            return {};
        }

        std::string _endpoint, _region;
        Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
    };

    TEST_F(SNSServerCliTest, TopicCreateTest) {

        // arrange

        // act
        const std::string output = Core::TestUtils::SendCliCommand("aws", {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

        // assert
        EXPECT_EQ(1, topicList.size());
        EXPECT_TRUE(Core::StringUtils::Contains(output, TEST_TOPIC));
    }

    TEST_F(SNSServerCliTest, TopicListTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"sns create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"sns", "list-topics", "--endpoint", _endpoint});
        Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

        // assert
        EXPECT_TRUE(Core::StringUtils::Contains(output2, TEST_TOPIC));
    }

    TEST_F(SNSServerCliTest, TopicSubscribeTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);

        const std::string output3 = Core::TestUtils::SendCliCommand("aws", {"sqs", "create-queue", "--queue-name", "test-queue", "--endpoint", _endpoint});
        const std::string queueUrl = GetQueueUrl(output3);

        // act
        const std::string output4 = Core::TestUtils::SendCliCommand("aws", {"sns", "subscribe", "--topic-arn", topicArn, "--protocol", "sqs", "--endpoint-url", queueUrl, "--endpoint", _endpoint});
        const std::string subscriptionArn = GetSubscriptionArn(output4);

        // assert
        EXPECT_FALSE(output4.empty());
        EXPECT_FALSE(subscriptionArn.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(subscriptionArn, "test-topic"));
    }

    TEST_F(SNSServerCliTest, TopicUnsubscribeTest) {

        // arrange
        // create topic
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint" + _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);

        // create queue
        const std::string output3 = Core::TestUtils::SendCliCommand("aws", {"sqs", "create-queue", "--queue-name", "test-queue", "--endpoint", _endpoint});
        const std::string queueUrl = GetQueueUrl(output3);

        // subscribe
        const std::string output4 = Core::TestUtils::SendCliCommand("aws", {"sns", "subscribe", "--topic-arn", topicArn, "--protocol", "sqs", "--endpoint-url", queueUrl, "--endpoint", _endpoint});
        const std::string subscriptionArn = GetSubscriptionArn(output4);

        // act
        const std::string output5 = Core::TestUtils::SendCliCommand("aws", {"sns", "unsubscribe", "--subscription-arn", subscriptionArn, "--endpoint", _endpoint});

        // assert
        EXPECT_TRUE(output5.empty());
    }

    TEST_F(SNSServerCliTest, TopicDeleteTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand("aws", {"sns", "delete-topic", "--topic-arn", topicArn, "--endpoint", _endpoint});
        const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

        // assert
        EXPECT_EQ(0, topicList.size());
    }

    TEST_F(SNSServerCliTest, MessagePublishTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand("aws", {"sns", "publish", "--topic-arn", topicArn, "--message", "TEST-BODY", "--endpoint", _endpoint});
        const long messageCount = _snsDatabase.CountMessages();
        const std::string messageId = GetMessageId(output3);

        // assert
        EXPECT_EQ(1, messageCount);
        EXPECT_FALSE(messageId.empty());
    }

    /*  TEST_F(SNSServerCliTest, MessageDeleteTest) {

    // arrange
    Core::ExecResult createResult = Core::TestUtils::SendCliCommand("sqs", "aws sqs create-queue --queue-name " + TEST_TOPIC + " --endpoint " + _endpoint);
    EXPECT_EQ(0, createResult.status);
    Core::ExecResult sendResult = Core::TestUtils::SendCliCommand("sqs", "aws sqs send-message --queue-url " + _queueUrl + " --message-body TEST-BODY --endpoint " + _endpoint);
    EXPECT_EQ(0, createResult.status);
    Core::ExecResult receiveResult = Core::TestUtils::SendCliCommand("sqs", "aws sqs receive-message --queue-url " + _queueUrl + " --endpoint " + _endpoint);
    EXPECT_EQ(0, receiveResult.status);
    std::string receiptHandle = GetReceiptHandle(receiveResult.output);

    // act
    Core::ExecResult result = Core::TestUtils::SendCliCommand("sqs", "aws sqs delete-message --queue-url " + _queueUrl + " --receipt-handle " + receiptHandle + " --endpoint " + _endpoint);
    EXPECT_EQ(0, receiveResult.status);
    long messageCount = _database.CountMessages(REGION, _queueUrl);

    // assert
    EXPECT_EQ(0, messageCount);
  }*/

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_SNS_CLI_INTEGRATION_TEST_H