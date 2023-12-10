//
// Created by vogje01 on 21/10/2023.
//

#ifndef AWMOCK_SNS_CLI_INTEGRATIONTEST_H
#define AWMOCK_SNS_CLI_INTEGRATIONTEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/Configuration.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/dto/sqs/CreateQueueResponse.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/SNSServer.h>
#include <awsmock/service/SQSServer.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define TEST_TOPIC std::string("test-topic")

namespace AwsMock::Service {

  class SNSServerCliTest : public ::testing::Test {

  protected:

    void SetUp() override {

      // Define endpoint. This is the endpoint of the SQS server, not the gateway
      std::string _snsPort = _configuration.getString("awsmock.service.sns.port", std::to_string(SNS_DEFAULT_PORT));
      std::string _snsHost = _configuration.getString("awsmock.service.sns.host", SNS_DEFAULT_HOST);
      std::string _sqsPort = _configuration.getString("awsmock.service.sqs.port", std::to_string(SQS_DEFAULT_PORT));
      std::string _sqsHost = _configuration.getString("awsmock.service.sqs.host", SQS_DEFAULT_HOST);
      _snsEndpoint = "http://" + _snsHost + ":" + _snsPort;
      _sqsEndpoint = "http://" + _sqsHost + ":" + _sqsPort;

      // Start HTTP services
      Poco::ThreadPool::defaultPool().start(_snsServer);
      Poco::ThreadPool::defaultPool().start(_sqsServer);
    }

    void TearDown() override {
      _snsDatabase.DeleteAllMessages();
      _snsDatabase.DeleteAllTopics();
      _sqsDatabase.DeleteAllMessages();
      _sqsDatabase.DeleteAllQueues();
      _sqsServer.StopServer();
      _snsServer.StopServer();
    }

    static std::string GetTopicArn(const std::string &jsonString) {

      std::string topicArn;
      Poco::JSON::Parser parser;
      Poco::Dynamic::Var result = parser.parse(jsonString);
      const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

      try {

        Poco::JSON::Array::Ptr topicsArray = rootObject->getArray("Topics");
        if (topicsArray != nullptr) {
          for (const auto &it : *topicsArray) {
            const auto& object =  it.extract<Poco::JSON::Object::Ptr>();
            Core::JsonUtils::GetJsonValueString("TopicArn", object, topicArn);
          }
        }

      } catch (Poco::Exception &exc) {
        throw Core::ServiceException(exc.message(), 500);
      }
      return topicArn;
    }

    static std::string GetMessageId(const std::string &jsonString) {

      std::string messageId;
      Poco::JSON::Parser parser;
      Poco::Dynamic::Var result = parser.parse(jsonString);
      const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

      try {

        Core::JsonUtils::GetJsonValueString("MessageId", rootObject, messageId);

      } catch (Poco::Exception &exc) {
        throw Core::ServiceException(exc.message(), 500);
      }
      return messageId;
    }

    static std::string GetQueueUrl(const std::string &jsonString) {

      std::string queueUrl;
      Poco::JSON::Parser parser;
      Poco::Dynamic::Var result = parser.parse(jsonString);
      const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

      try {

        Core::JsonUtils::GetJsonValueString("QueueUrl", rootObject, queueUrl);

      } catch (Poco::Exception &exc) {
        throw Core::ServiceException(exc.message(), 500);
      }
      return queueUrl;
    }

    static std::string GetSubscriptionArn(const std::string &jsonString) {

      std::string subscriptionArn;
      Poco::JSON::Parser parser;
      Poco::Dynamic::Var result = parser.parse(jsonString);
      const auto &rootObject = result.extract<Poco::JSON::Object::Ptr>();

      try {

        Core::JsonUtils::GetJsonValueString("SubscriptionArn", rootObject, subscriptionArn);

      } catch (Poco::Exception &exc) {
        throw Core::ServiceException(exc.message(), 500);
      }
      return subscriptionArn;
    }

    std::string _snsEndpoint, _sqsEndpoint;
    Core::Configuration _configuration = Core::TestUtils::GetTestConfiguration();
    Core::MetricService _metricService = Core::MetricService(_configuration);
    Database::SNSDatabase _snsDatabase = Database::SNSDatabase(_configuration);
    Database::SQSDatabase _sqsDatabase = Database::SQSDatabase(_configuration);
    SNSServer _snsServer = SNSServer(_configuration, _metricService);
    SQSServer _sqsServer = SQSServer(_configuration, _metricService);
  };

  TEST_F(SNSServerCliTest, TopicCreateTest) {

    // arrange

    // act
    Core::ExecResult createResult = Core::TestUtils::SendCliCommand("sns", "aws sns create-topic --name " + TEST_TOPIC + " --endpoint " + _snsEndpoint);
    Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

    // assert
    EXPECT_EQ(0, createResult.status);
    EXPECT_EQ(1, topicList.size());
    EXPECT_TRUE(Core::StringUtils::Contains(createResult.output, TEST_TOPIC));
  }

  TEST_F(SNSServerCliTest, TopicListTest) {

    // arrange
    Core::ExecResult createResult = Core::TestUtils::SendCliCommand("sns", "aws sns create-topic --name " + TEST_TOPIC + " --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, createResult.status);

    // act
    Core::ExecResult listResult = Core::TestUtils::SendCliCommand("_snsEndpoint", "aws sns list-topics --endpoint " + _snsEndpoint);
    Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

    // assert
    EXPECT_EQ(0, listResult.status);
    EXPECT_TRUE(Core::StringUtils::Contains(listResult.output, TEST_TOPIC));
  }

  TEST_F(SNSServerCliTest, TopicSubscribeTest) {

    // arrange
    Core::ExecResult createTopicResult = Core::TestUtils::SendCliCommand("sns", "aws sns create-topic --name " + TEST_TOPIC + " --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, createTopicResult.status);
    Core::ExecResult listResult = Core::TestUtils::SendCliCommand("_snsEndpoint", "aws sns list-topics --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, listResult.status);
    std::string topicArn = GetTopicArn(listResult.output);

    Core::ExecResult createQueueResult = Core::TestUtils::SendCliCommand("sqs", "aws sqs create-queue --queue-name test-queue --endpoint " + _sqsEndpoint);
    EXPECT_EQ(0, createQueueResult.status);
    std::string queueUrl = GetQueueUrl(createQueueResult.output);

    // act
    Core::ExecResult subscribeResult = Core::TestUtils::SendCliCommand("sns", "aws sns subscribe --topic-arn " + topicArn + " --protocol sqs --endpoint-url " + queueUrl + " --endpoint " + _snsEndpoint);
    std::string subscriptionArn = GetSubscriptionArn(subscribeResult.output);

    // assert
    EXPECT_EQ(0, subscribeResult.status);
    EXPECT_TRUE(subscribeResult.output.length() > 0);
    EXPECT_FALSE(subscriptionArn.empty());
    EXPECT_TRUE(Core::StringUtils::Contains(subscriptionArn, "test-topic"));
  }

  TEST_F(SNSServerCliTest, TopicUnsubscribeTest) {

    // arrange
    // create topic
    Core::ExecResult createTopicResult = Core::TestUtils::SendCliCommand("sns", "aws sns create-topic --name " + TEST_TOPIC + " --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, createTopicResult.status);
    Core::ExecResult listResult = Core::TestUtils::SendCliCommand("_snsEndpoint", "aws sns list-topics --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, listResult.status);
    std::string topicArn = GetTopicArn(listResult.output);

    // create queue
    Core::ExecResult createQueueResult = Core::TestUtils::SendCliCommand("sqs", "aws sqs create-queue --queue-name test-queue --endpoint " + _sqsEndpoint);
    EXPECT_EQ(0, createQueueResult.status);
    std::string queueUrl = GetQueueUrl(createQueueResult.output);

    // subscribe
    Core::ExecResult subscribeResult = Core::TestUtils::SendCliCommand("sns", "aws sns subscribe --topic-arn " + topicArn + " --protocol sqs --endpoint-url " + queueUrl + " --endpoint " + _snsEndpoint);
    std::string subscriptionArn = GetSubscriptionArn(subscribeResult.output);

    // act
    Core::ExecResult unsubscribeResult = Core::TestUtils::SendCliCommand("sns", "aws sns unsubscribe --subscription-arn " + subscriptionArn + " --endpoint " + _snsEndpoint);

    // assert
    EXPECT_EQ(0, unsubscribeResult.status);
    EXPECT_TRUE(unsubscribeResult.output.empty());
  }

  TEST_F(SNSServerCliTest, TopicDeleteTest) {

    // arrange
    Core::ExecResult createResult = Core::TestUtils::SendCliCommand("sns", "aws sns create-topic --name " + TEST_TOPIC + " --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, createResult.status);
    Core::ExecResult listResult = Core::TestUtils::SendCliCommand("sns", "aws sns list-topics --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, listResult.status);
    std::string topicArn = GetTopicArn(listResult.output);

    // act
    Core::ExecResult result = Core::TestUtils::SendCliCommand("sns", "aws sns delete-topic --topic-arn " + topicArn + " --endpoint " + _snsEndpoint);
    Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

    // assert
    EXPECT_EQ(0, result.status);
    EXPECT_EQ(0, topicList.size());
  }

  TEST_F(SNSServerCliTest, MessagePublishTest) {

    // arrange
    Core::ExecResult createResult = Core::TestUtils::SendCliCommand("sns", "aws sns create-topic --name " + TEST_TOPIC + " --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, createResult.status);
    Core::ExecResult listResult = Core::TestUtils::SendCliCommand("sns", "aws sns list-topics --endpoint " + _snsEndpoint);
    EXPECT_EQ(0, listResult.status);
    std::string topicArn = GetTopicArn(listResult.output);

    // act
    Core::ExecResult result = Core::TestUtils::SendCliCommand("sns", "aws sns publish --topic-arn " + topicArn + " --message TEST-BODY --endpoint " + _snsEndpoint);
    long messageCount = _snsDatabase.CountMessages();
    std::string messageId = GetMessageId(result.output);

    // assert
    EXPECT_EQ(0, result.status);
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

} // namespace AwsMock::Core

#endif // AWMOCK_SNS_CLI_INTEGRATIONTEST_H