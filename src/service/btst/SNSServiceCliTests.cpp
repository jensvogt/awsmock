//
// Created by vogje01 on 21/10/2023.
//

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

#define BOOST_TEST_MODULE SNSServiceCliTests
#define TEST_TOPIC std::string("test-topic")
#define TEST_QUEUE std::string("test-queue")
#define REGION std::string("eu-central-1")

namespace AwsMock::Service {

    /**
     * @brief Test the SNS command line interface of AwsMock.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SNSServerCliTest : TestBase {

        SNSServerCliTest() {

            // General configuration
            StartGateway();
            _region = GetRegion();
            _endpoint = GetEndpoint();

            // Cleanup
            long deleted = _snsDatabase.DeleteAllMessages();
            log_debug << "Deleted SNS messages, count: " << deleted;
            deleted = _snsDatabase.DeleteAllTopics();
            log_debug << "Deleted SNS topics, count: " << deleted;
            deleted = _sqsDatabase.DeleteAllMessages();
            log_debug << "Deleted SQS messages, count: " << deleted;
            deleted = _sqsDatabase.DeleteAllQueues();
            log_debug << "Deleted SQS queues, count: " << deleted;
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

    BOOST_FIXTURE_TEST_CASE(TopicCreateTest, SNSServerCliTest) {

        // arrange

        // act
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

        // assert
        BOOST_CHECK_EQUAL(1, topicList.size());
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output, TEST_TOPIC), true);
    }

    BOOST_FIXTURE_TEST_CASE(TopicListTest, SNSServerCliTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "list-topics", "--endpoint", _endpoint});
        Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

        // assert
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output2, TEST_TOPIC), true);
    }

    BOOST_FIXTURE_TEST_CASE(TopicSubscribeTest, SNSServerCliTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", "test-queue", "--endpoint", _endpoint});
        const std::string queueUrl = GetQueueUrl(output3);

        // act
        const std::string output4 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "subscribe", "--topic-arn", topicArn, "--protocol", "sqs", "--endpoint-url", queueUrl, "--endpoint", _endpoint});
        const std::string subscriptionArn = GetSubscriptionArn(output4);

        // assert
        BOOST_CHECK_EQUAL(output4.empty(), false);
        BOOST_CHECK_EQUAL(subscriptionArn.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(subscriptionArn, "test-topic"), true);
    }

    BOOST_FIXTURE_TEST_CASE(TopicUnsubscribeTest, SNSServerCliTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);

        // create queue
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", "test-queue", "--endpoint", _endpoint});
        const std::string queueUrl = GetQueueUrl(output3);

        // subscribe
        const std::string output4 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "subscribe", "--topic-arn", topicArn, "--protocol", "sqs", "--endpoint-url", queueUrl, "--endpoint", _endpoint});
        const std::string subscriptionArn = GetSubscriptionArn(output4);

        // act
        const std::string output5 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "unsubscribe", "--subscription-arn", subscriptionArn, "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(output5.empty(), true);
    }

    BOOST_FIXTURE_TEST_CASE(TopicDeleteTest, SNSServerCliTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "delete-topic", "--topic-arn", topicArn, "--endpoint", _endpoint});
        const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

        // assert
        BOOST_CHECK_EQUAL(0, topicList.size());
    }

    BOOST_FIXTURE_TEST_CASE(MessagePublishTest, SNSServerCliTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "publish", "--topic-arn", topicArn, "--message", "TEST-BODY", "--endpoint", _endpoint});
        const long messageCount = _snsDatabase.CountMessages();
        const std::string messageId = GetMessageId(output3);

        // assert
        BOOST_CHECK_EQUAL(1, messageCount);
        BOOST_CHECK_EQUAL(messageId.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(MessageReceiveTest, SNSServerCliTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "create-topic", "--name", TEST_TOPIC, "--endpoint", _endpoint});
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "list-topics", "--endpoint", _endpoint});
        const std::string topicArn = GetTopicArn(output2);
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueArn = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueArn;
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;
        const std::string output4 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "subscribe", "--topic-arn", topicArn, "--protocol", "sqs", "--notification-endpoint", queueArn, "--endpoint", _endpoint});
        const std::string subscriptionArn = GetSubscriptionArn(output4);

        // act
        const std::string output5 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sns", "publish", "--topic-arn", topicArn, "--message", "TEST-BODY", "--endpoint", _endpoint});
        const long messageCount = _snsDatabase.CountMessages();
        const std::string output6 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "receive-message", "--queue-url", queueUrl, "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(1, messageCount);
        BOOST_CHECK_EQUAL(output6.empty(), false);
    }

}// namespace AwsMock::Service
