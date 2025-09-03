//
// Created by vogje01 on 21/10/2023.
//

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define BOOST_TEST_MODULE SQSServiceCliTests
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
    struct SQSServiceCliTests : TestBase {

        SQSServiceCliTests() {

            // General configuration
            StartGateway();
            _region = GetRegion();
            _endpoint = GetEndpoint();

            // ReSharper disable once CppExpressionWithoutSideEffects
            long deleted = _sqsDatabase.DeleteAllMessages();
            log_debug << "Deleted messages, count: " << deleted;
            // ReSharper disable once CppExpressionWithoutSideEffects
            deleted = _sqsDatabase.DeleteAllQueues();
            log_debug << "Deleted messages, count: " << deleted;
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

        std::string _endpoint, _region;
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(QueueCreateTest, SQSServiceCliTests) {

        // arrange

        // act
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();

        // assert
        BOOST_CHECK_EQUAL(1, queueList.size());
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output, TEST_QUEUE), true);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListTest, SQSServiceCliTests) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});

        // act
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "list-queues", "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output1, TEST_QUEUE), true);
    }

    BOOST_FIXTURE_TEST_CASE(QueueGetUrlTest, SQSServiceCliTests) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});

        // act
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "get-queue-url", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output1, TEST_QUEUE), true);
    }

    BOOST_FIXTURE_TEST_CASE(QueuePurgeTest, SQSServiceCliTests) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "purge-queue", "--queue-url", queueUrl, "--endpoint", _endpoint});
        const long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl));

        // assert
        BOOST_CHECK_EQUAL(0, messageCount);
    }

    BOOST_FIXTURE_TEST_CASE(QueueDeleteTest, SQSServiceCliTests) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "get-queue-url", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "delete-queue", "--queue-url", queueUrl, "--endpoint", _endpoint});
        const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();

        // assert
        BOOST_CHECK_EQUAL(0, queueList.size());
    }

    BOOST_FIXTURE_TEST_CASE(MessageSendTest, SQSServiceCliTests) {

        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        // act
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});
        const long messageCount = _sqsDatabase.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl));

        // assert
        BOOST_CHECK_EQUAL(1, messageCount);
    }

    BOOST_FIXTURE_TEST_CASE(MessageReceiveTest, SQSServiceCliTests) {
        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;

        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "receive-message", "--queue-url", queueUrl, "--endpoint", _endpoint});

        // assert
        BOOST_CHECK_EQUAL(output3.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(output3, "Messages"), true);
    }

    BOOST_FIXTURE_TEST_CASE(MessageDeleteTest, SQSServiceCliTests) {
        // arrange
        const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "create-queue", "--queue-name", TEST_QUEUE, "--endpoint", _endpoint});
        const std::string queueUrl = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueUrl;
        const std::string queueArn = _sqsDatabase.GetQueueByName(REGION, TEST_QUEUE).queueArn;

        const std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "send-message", "--queue-url", queueUrl, "--message-body", "TEST-BODY", "--endpoint", _endpoint});

        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "receive-message", "--queue-url", queueUrl, "--endpoint", _endpoint});
        const std::string receiptHandle = GetReceiptHandle(output2);

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"sqs", "delete-message", "--queue-url", queueUrl, "--receipt-handle", receiptHandle, "--endpoint", _endpoint});
        const long messageCount = _sqsDatabase.CountMessages(queueArn);

        // assert
        BOOST_CHECK_EQUAL(0, messageCount);
    }

}// namespace AwsMock::Service
