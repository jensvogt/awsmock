//
// Created by vogje01 on 21/10/2023.
//

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/sqs/SQSServer.h>

#define BOOST_TEST_MODULE SQSServiceJavaTests
#define TEST_QUEUE std::string("test-queue")
#define TEST_MESSAGE std::string("{\"testKey\": \"testValue\"}")
#define TEST_PORT 10100

namespace AwsMock::Service {

    struct TestMessage {

        std::string testKey;
        std::string receiptHandle;

        [[nodiscard]] std::string ToJson() const {

            document rootDocument;
            rootDocument.append(kvp("testKey", testKey));
            rootDocument.append(kvp("receiptHandle", receiptHandle));
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);
        }

        void FromJson(const std::string &jsonString) {

            const value rootDocument = bsoncxx::from_json(jsonString);
            testKey = Core::Bson::BsonUtils::GetStringValue(rootDocument, "testKey");
            receiptHandle = Core::Bson::BsonUtils::GetStringValue(rootDocument, "receiptHandle");
        }
    };

    struct TestMessageList {

        std::vector<TestMessage> messageList;

        std::string ToJson() {
            array rootArray;
            for (const auto &[testKey, receiptHandle]: messageList) {
                document elementDocument;
                elementDocument.append(kvp("testKey", testKey));
                elementDocument.append(kvp("receiptHandle", receiptHandle));
                rootArray.append(elementDocument);
            }
            return Core::Bson::BsonUtils::ToJsonString(rootArray);
        }

        void FromJson(const std::string &jsonString) {

            if (const value rootDocument = bsoncxx::from_json(jsonString); rootDocument.view().find("Messages") != rootDocument.view().end()) {
                messageList = std::vector<TestMessage>();
                array array;
                for (const auto &element: rootDocument["Messages"].get_array().value) {
                    TestMessage testMessage;
                    testMessage.testKey = Core::Bson::BsonUtils::GetStringValue(element.get_document().value, "testKey");
                    testMessage.receiptHandle = Core::Bson::BsonUtils::GetStringValue(element.get_document().value, "receiptHandle");
                    messageList.emplace_back(testMessage);
                }
            }
        }
    };

    /**
     * @brief Tests the aws-sdk-java interface to the AwsMock system.
     *
     * @par
     * The awsmock-test docker image will be started. The SNS server as well as the SQS server are started. This is needed as the SNS topic subscribe command needs an existing SQS queue.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SQSServiceJavaTest : TestBase {

        SQSServiceJavaTest() {

            // Start the gateway server
            StartGateway(TEST_PORT);

            // General configuration
            _region = GetRegion();

            // Base URL
            _baseUrl = "/api/sqs/";
        }

        ~SQSServiceJavaTest() {
            // ReSharper disable once CppExpressionWithoutSideEffects
            _sqsDatabase.DeleteAllMessages();

            // ReSharper disable once CppExpressionWithoutSideEffects
            _sqsDatabase.DeleteAllQueues();
        }

        static Core::HttpSocketResponse SendPostCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        static Core::HttpSocketResponse SendGetCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::get, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        static Core::HttpSocketResponse SendDeleteCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::delete_, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        std::string _region, _baseUrl;
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
    };

    BOOST_FIXTURE_TEST_CASE(SQSCreateQueueTest, SQSServiceJavaTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        const std::string queueUrl = result.body;

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(queueUrl.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(queueUrl, TEST_QUEUE), true);
    }

    BOOST_FIXTURE_TEST_CASE(SQSGetQueueUrlTest, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, result.statusCode);

        // act
        const Core::HttpSocketResponse getUrlResult = SendGetCommand(_baseUrl + "getQueueUrl?queueName=" + Core::StringUtils::UrlEncode(TEST_QUEUE), {});
        const std::string queueUrl = result.body;

        // assert
        BOOST_CHECK_EQUAL(getUrlResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(queueUrl.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(queueUrl, TEST_QUEUE), true);
    }

    BOOST_FIXTURE_TEST_CASE(SQSGetAllQueueAttributes, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, result.statusCode);
        const std::string queueUrl = result.body;

        // act
        const Core::HttpSocketResponse resultQueueAttributes = SendPostCommand(_baseUrl + "getAllQueueAttributes?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), {});
        const std::string queueAttributes = resultQueueAttributes.body;

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(queueAttributes.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SQSGetSingleQueueAttributes, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, result.statusCode);
        const std::string queueUrl = result.body;

        // act
        const Core::HttpSocketResponse resultQueueAttributes = SendGetCommand(_baseUrl + "getSingleQueueAttribute?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&attributeName=" + Core::StringUtils::UrlEncode("APPROXIMATE_NUMBER_OF_MESSAGES"), {});
        const std::string queueAttributes = resultQueueAttributes.body;

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(queueAttributes.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SQSSetQueueAttributes, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, result.statusCode);
        const std::string queueUrl = result.body;

        // act
        const Core::HttpSocketResponse resultQueueAttributes = SendPostCommand(_baseUrl + "setQueueAttribute?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&attributeValue=3600", {});

        // assert
        BOOST_CHECK_EQUAL(resultQueueAttributes.statusCode == http::status::ok, true);
    }

    BOOST_FIXTURE_TEST_CASE(SQSChangeMessageVisibilityTest, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, result.statusCode);
        const std::string queueUrl = result.body;
        const TestMessage testMessage = {.testKey = "testKey"};
        Core::HttpSocketResponse sendResult = SendPostCommand(_baseUrl + "sendMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());
        const Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();
        const std::string receiptHandle = messageList[0].receiptHandle;

        // act
        const Core::HttpSocketResponse resultQueueAttributes = SendPostCommand(_baseUrl + "changeVisibility?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&visibility=3600&receiptHandle=" + Core::StringUtils::UrlEncode(receiptHandle), {});

        // assert
        BOOST_CHECK_EQUAL(resultQueueAttributes.statusCode == http::status::ok, true);
    }

    BOOST_FIXTURE_TEST_CASE(SQSSendMessageTest, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, result.statusCode);
        const std::string queueUrl = result.body;

        // act
        const TestMessage testMessage = {.testKey = "testKey"};
        Core::HttpSocketResponse sendResult = SendPostCommand(_baseUrl + "sendMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());
        const Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, messageList.size());
    }

    BOOST_FIXTURE_TEST_CASE(SQSSendMessageAttributeTest, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        const std::string queueUrl = result.body;

        // act
        const TestMessage testMessage = {.testKey = "testKey"};
        Core::HttpSocketResponse sendResult = SendPostCommand(_baseUrl + "sendMessageAttributes?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);

        Core::HttpSocketResponse receiveResult = SendGetCommand(_baseUrl + "receiveMessages?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&maxMessages=10&maxWaitTime=5", {});
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        const Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();

        BOOST_CHECK_EQUAL(1, messageList.size());
        const Database::Entity::SQS::Message message = messageList.front();

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, messageList.size());
        BOOST_CHECK_EQUAL(5, message.attributes.size());
    }

    BOOST_FIXTURE_TEST_CASE(SQSMessageReceiveTest, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        const std::string queueUrl = result.body;
        const TestMessage testMessage = {.testKey = "testKey"};
        Core::HttpSocketResponse sendResult = SendPostCommand(_baseUrl + "sendMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());

        // act
        Core::HttpSocketResponse receiveResult = SendGetCommand(_baseUrl + "receiveMessages?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&maxMessages=10&maxWaitTime=5", {});
        const Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();
        BOOST_CHECK_EQUAL(1, messageList.size());
        const Database::Entity::SQS::Message message = messageList.front();

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, messageList.size());
        BOOST_CHECK_EQUAL(message.status == Database::Entity::SQS::MessageStatus::INVISIBLE, true);
    }

    BOOST_FIXTURE_TEST_CASE(SQSPurgeQueueTest, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, result.statusCode);
        const std::string queueUrl = result.body;
        const TestMessage testMessage = {.testKey = "testKey"};
        Core::HttpSocketResponse sendResult = SendPostCommand(_baseUrl + "sendMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());

        // act
        Core::HttpSocketResponse purgeResult = SendGetCommand(_baseUrl + "purgeQueue?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), {});
        const Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(0, messageList.size());
    }

    BOOST_FIXTURE_TEST_CASE(SQSTemplateTest, SQSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, createResult.statusCode);
        std::string queueUrl = createResult.body;

        // act
        const TestMessage testMessage = {.testKey = "testKey"};
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "sqsTemplate", testMessage.ToJson());
        const Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, messageList.size());
    }

    BOOST_FIXTURE_TEST_CASE(SQSDeleteMessageTest, SQSServiceJavaTest) {

        // arrange
        TestMessage testMessage = {.testKey = "testKey"};
        Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, createResult.statusCode);
        std::string queueUrl = createResult.body;
        Core::HttpSocketResponse sendResult = SendPostCommand(_baseUrl + "sendMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());
        BOOST_CHECK_EQUAL(http::status::ok, sendResult.statusCode);
        std::string messageId = sendResult.body;
        Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();
        BOOST_CHECK_EQUAL(1, messageList.size());

        // act
        std::string receiptHandle = messageList[0].receiptHandle;
        Core::HttpSocketResponse deleteResult = SendDeleteCommand(_baseUrl + "deleteMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&receiptHandle=" + Core::StringUtils::UrlEncode(receiptHandle), {});
        messageList = _sqsDatabase.ListMessages();

        // assert
        BOOST_CHECK_EQUAL(deleteResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(0, messageList.size());
    }

    BOOST_FIXTURE_TEST_CASE(SQSDeleteMessageBatchTest, SQSServiceJavaTest) {

        // arrange
        TestMessage testMessage = {.testKey = "testKey"};
        Core::HttpSocketResponse createResult = SendPostCommand(_baseUrl + "createQueue?queueName=" + TEST_QUEUE, {});
        BOOST_CHECK_EQUAL(http::status::ok, createResult.statusCode);
        std::string queueUrl = createResult.body;
        Core::HttpSocketResponse sendResult1 = SendPostCommand(_baseUrl + "sendMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());
        BOOST_CHECK_EQUAL(http::status::ok, sendResult1.statusCode);
        Core::HttpSocketResponse sendResult2 = SendPostCommand(_baseUrl + "sendMessage?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl), testMessage.ToJson());
        BOOST_CHECK_EQUAL(http::status::ok, sendResult2.statusCode);
        Database::Entity::SQS::MessageList messageList = _sqsDatabase.ListMessages();
        BOOST_CHECK_EQUAL(2, messageList.size());

        // act
        std::string receiptHandle1 = messageList[0].receiptHandle;
        std::string receiptHandle2 = messageList[1].receiptHandle;
        Core::HttpSocketResponse deleteResult = SendDeleteCommand(_baseUrl + "deleteMessageBatch?queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&receiptHandle1=" + Core::StringUtils::UrlEncode(receiptHandle1) + "&receiptHandle2=" + Core::StringUtils::UrlEncode(receiptHandle2), {});
        messageList = _sqsDatabase.ListMessages();

        // assert
        BOOST_CHECK_EQUAL(deleteResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(0, messageList.size());
    }

}// namespace AwsMock::Service
