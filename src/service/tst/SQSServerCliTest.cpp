//
// Created by vogje01 on 21/10/2023.
//

#ifndef AWMOCK_SQS_CLI_INTEGRATIONTEST_H
#define AWMOCK_SQS_CLI_INTEGRATIONTEST_H

// GTest includes
#include <gtest/gtest.h>

// Boost includes
#include <boost/asio/thread_pool.hpp>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/dto/sqs/CreateQueueResponse.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/sqs/SQSServer.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define SQS_ACCOUNT_ID "000000000000"
#define TEST_QUEUE std::string("test-queue")

namespace AwsMock::Service {

    class SQSServerCliTest : public ::testing::Test {

      protected:

        void SetUp() override {

            // Define endpoint. This is the endpoint of the SQS server, not the gateway
            std::string _region = _configuration.getString("awsmock.region");
            std::string _port = _configuration.getString("awsmock.service.sqs.http.port", std::to_string(GATEWAY_DEFAULT_PORT));
            std::string _host = _configuration.getString("awsmock.service.sqs.http.host", GATEWAY_DEFAULT_HOST);

            // Set test config
            _configuration.setString("awsmock.service.gateway.http.port", _port);
            _accountId = _configuration.getString("awsmock.account.id", SQS_ACCOUNT_ID);
            _endpoint = "http://" + _host + ":" + _port;
            _queueUrl = "http://sqs." + _region + "." + Core::SystemUtils::GetHostName() + ":" + _port + "/" + _accountId + "/" + TEST_QUEUE;

            // Start HTTP manager
            // Start gateway server
            _gatewayServer = std::make_shared<Service::GatewayServer>(_ios);
            _thread = boost::thread([&]() {
                boost::asio::io_service::work work(_ios);
                _ios.run();
            });
        }

        void TearDown() override {
            _sqsDatabase.DeleteAllMessages();
            _sqsDatabase.DeleteAllQueues();
            _ios.stop();
            _thread.join();
        }

        static std::string GetReceiptHandle(const std::string &jsonString) {

            std::string receiptHandle;
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(jsonString);
            const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

            try {
                Poco::JSON::Array::Ptr messageArray = rootObject->getArray("Messages");

                if (messageArray != nullptr) {
                    for (const auto &it: *messageArray) {
                        const auto &object = it.extract<Poco::JSON::Object::Ptr>();
                        Core::JsonUtils::GetJsonValueString("ReceiptHandle", object, receiptHandle);
                    }
                }
            } catch (Poco::Exception &exc) {
                throw Core::ServiceException(exc.message());
            }
            return receiptHandle;
        }

        boost::thread _thread;
        boost::asio::io_service _ios{10};
        std::string _endpoint, _queueUrl, _accountId;
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
        std::shared_ptr<Service::GatewayServer> _gatewayServer;
    };

    TEST_F(SQSServerCliTest, QueueCreateTest) {

        // arrange

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();

        // assert
        EXPECT_EQ(0, result.status);
        EXPECT_EQ(1, queueList.size());
        EXPECT_TRUE(result.output.empty());
    }

    TEST_F(SQSServerCliTest, QueueListTest) {

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs list-queues --endpoint " + _endpoint);

        // assert
        EXPECT_EQ(0, result.status);
        EXPECT_TRUE(Core::StringUtils::Contains(result.output, TEST_QUEUE));
    }

    TEST_F(SQSServerCliTest, QueueGetUrlTest) {

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs get-queue-url --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);

        // assert
        EXPECT_EQ(0, result.status);
        EXPECT_TRUE(Core::StringUtils::Contains(result.output, TEST_QUEUE));
    }

    TEST_F(SQSServerCliTest, QueuePurgeTest) {

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);

        Core::ExecResult sendResult = Core::TestUtils::SendCliCommand("aws sqs send-message --queue-url " + _queueUrl + " --message-body TEST-BODY --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs purge-queue --queue-url " + _queueUrl + " --endpoint " + _endpoint);
        long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, _queueUrl));

        // assert
        EXPECT_EQ(0, messageCount);
    }

    TEST_F(SQSServerCliTest, QueueDeleteTest) {

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);
        Core::ExecResult getQueueUrlResult = Core::TestUtils::SendCliCommand("aws sqs get-queue-url --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs delete-queue --queue-url " + queueUrl + " --endpoint " + _endpoint);
        Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();

        // assert
        EXPECT_EQ(0, result.status);
        EXPECT_EQ(0, queueList.size());
    }

    TEST_F(SQSServerCliTest, MessageSendTest) {

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;
        EXPECT_EQ(0, createResult.status);

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs send-message --queue-url " + queueUrl + " --message-body TEST-BODY --endpoint " + _endpoint);
        long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl));

        // assert
        EXPECT_EQ(0, result.status);
        EXPECT_EQ(1, messageCount);
    }

    TEST_F(SQSServerCliTest, MessageReceiveTest) {

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);
        std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        Core::ExecResult sendResult = Core::TestUtils::SendCliCommand("aws sqs send-message --queue-url " + queueUrl + " --message-body TEST-BODY --endpoint " + _endpoint);
        EXPECT_EQ(0, sendResult.status);

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs receive-message --queue-url " + queueUrl + " --endpoint " + _endpoint);

        // assert
        EXPECT_EQ(0, result.status);
        EXPECT_FALSE(result.output.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(result.output, "Messages"));
    }

    TEST_F(SQSServerCliTest, MessageDeleteTest) {

        // arrange
        Core::ExecResult createResult = Core::TestUtils::SendCliCommand("aws sqs create-queue --queue-name " + TEST_QUEUE + " --endpoint " + _endpoint);
        EXPECT_EQ(0, createResult.status);
        std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        Core::ExecResult sendResult = Core::TestUtils::SendCliCommand("aws sqs send-message --queue-url " + queueUrl + " --message-body TEST-BODY --endpoint " + _endpoint);
        EXPECT_EQ(0, sendResult.status);

        Core::ExecResult receiveResult = Core::TestUtils::SendCliCommand("aws sqs receive-message --queue-url " + queueUrl + " --endpoint " + _endpoint);
        EXPECT_EQ(0, receiveResult.status);
        std::string receiptHandle = GetReceiptHandle(receiveResult.output);

        // act
        Core::ExecResult result = Core::TestUtils::SendCliCommand("aws sqs delete-message --queue-url " + queueUrl + " --receipt-handle " + receiptHandle + " --endpoint " + _endpoint);
        EXPECT_EQ(0, receiveResult.status);
        long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl));

        // assert
        EXPECT_EQ(0, messageCount);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SQS_CLI_INTEGRATIONTEST_H