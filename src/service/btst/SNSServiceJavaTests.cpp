//
// Created by vogje01 on 21/10/2023.
//

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/sns/SNSServer.h>
#include <awsmock/service/sqs/SQSServer.h>

#define TEST_PORT 10100
#define TEST_TOPIC std::string("test-topic")
#define TEST_QUEUE std::string("test-queue")
#define TEST_MESSAGE_JSON std::string("{\"test-message\":\"test-value\"")

namespace AwsMock::Service {

    /**
     * @brief Tests the aws-sdk-java interface to the AwsMock system.
     *
     * @par
     * The awsmock-test docker image will be started. The SNS server as well as the SQS server are started. This is needed as the SNS topic subscribe command needs an existing SQS queue.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SNSServiceJavaTest : TestBase {

      protected:

        SNSServiceJavaTest() {

            // Start gateway server
            StartGateway(TEST_PORT);

            // General configuration
            _region = GetRegion();

            // Base URL
            _snsBaseUrl = "/api/sns/";
            _sqsBaseUrl = "/api/sqs/";

            // Cleanup
            long deleted = _snsDatabase.DeleteAllMessages();
            log_debug << "SNS messages deleted, count: " << deleted;
            deleted = _snsDatabase.DeleteAllTopics();
            log_debug << "SNS topics deleted, count: " << deleted;
            deleted = _sqsDatabase.DeleteAllMessages();
            log_debug << "SQS messages deleted, count: " << deleted;
            deleted = _sqsDatabase.DeleteAllQueues();
            log_debug << "SQS queues deleted, count: " << deleted;
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

        std::string _snsBaseUrl, _sqsBaseUrl, _region;
        Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
        std::shared_ptr<GatewayServer> _gatewayServer;
    };

    BOOST_FIXTURE_TEST_CASE(TopicCreateTest, SNSServiceJavaTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_snsBaseUrl + "createTopic?name=" + Core::StringUtils::UrlEncode(TEST_TOPIC), {});
        const std::string topicArn = result.body;

        // assert
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(topicArn.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(topicArn, TEST_TOPIC), true);
    }

    BOOST_FIXTURE_TEST_CASE(TopicListTest, SNSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_snsBaseUrl + "createTopic?name=" + Core::StringUtils::UrlEncode(TEST_TOPIC), {});
        BOOST_CHECK_EQUAL(result.statusCode == http::status::ok, true);
        std::string topicArn = result.body;

        // act
        const Core::HttpSocketResponse listResult = SendGetCommand(_snsBaseUrl + "listTopics", {});
        BOOST_CHECK_EQUAL(listResult.statusCode == http::status::ok, true);

        // assert
        BOOST_CHECK_EQUAL(1, std::stoi(listResult.body));
    }

    BOOST_FIXTURE_TEST_CASE(TopicSubscribeTest, SNSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse createTopicResult = SendPostCommand(_snsBaseUrl + "createTopic?name=" + Core::StringUtils::UrlEncode(TEST_TOPIC), {});
        BOOST_CHECK_EQUAL(createTopicResult.statusCode == http::status::ok, true);
        const std::string topicArn = createTopicResult.body;
        const Core::HttpSocketResponse createQueueResult = SendPostCommand(_sqsBaseUrl + "createQueue?queueName=" + Core::StringUtils::UrlEncode(TEST_QUEUE), {});
        BOOST_CHECK_EQUAL(createQueueResult.statusCode == http::status::ok, true);
        const std::string queueUrl = createQueueResult.body;

        // act
        const Core::HttpSocketResponse subscribeResult = SendPostCommand(_snsBaseUrl + "subscribe?topicArn=" + Core::StringUtils::UrlEncode(topicArn) + "&queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&protocol=SQS", {});
        const std::string subscriptionArn = subscribeResult.body;

        // assert
        BOOST_CHECK_EQUAL(subscribeResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(subscriptionArn.empty(), false);
        BOOST_CHECK_EQUAL(Core::StringUtils::Contains(subscriptionArn, "test-topic"), true);
    }

    BOOST_FIXTURE_TEST_CASE(TopicUnsubscribeTest, SNSServiceJavaTest) {

        // arrange
        Core::HttpSocketResponse createTopicResult = SendPostCommand(_snsBaseUrl + "createTopic?name=" + Core::StringUtils::UrlEncode(TEST_TOPIC), {});
        BOOST_CHECK_EQUAL(createTopicResult.statusCode == http::status::ok, true);
        std::string topicArn = createTopicResult.body;
        Core::HttpSocketResponse createQueueResult = SendPostCommand(_sqsBaseUrl + "createQueue?queueName=" + Core::StringUtils::UrlEncode(TEST_QUEUE), {});
        BOOST_CHECK_EQUAL(createQueueResult.statusCode == http::status::ok, true);
        std::string queueUrl = createQueueResult.body;
        Core::HttpSocketResponse subscribeResult = SendPostCommand(_snsBaseUrl + "subscribe?topicArn=" + Core::StringUtils::UrlEncode(topicArn) + "&queueUrl=" + Core::StringUtils::UrlEncode(queueUrl) + "&protocol=SQS", {});
        std::string subscriptionArn = subscribeResult.body;

        // act
        Core::HttpSocketResponse unsubscribeResult = SendPostCommand(_snsBaseUrl + "unsubscribe?subscriptionArn=" + Core::StringUtils::UrlEncode(subscriptionArn), {});

        // assert
        BOOST_CHECK_EQUAL(unsubscribeResult.statusCode == http::status::ok, true);
    }

    BOOST_FIXTURE_TEST_CASE(TopicPublishTest, SNSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse createTopicResult = SendPostCommand(_snsBaseUrl + "createTopic?name=" + Core::StringUtils::UrlEncode(TEST_TOPIC), {});
        BOOST_CHECK_EQUAL(createTopicResult.statusCode == http::status::ok, true);
        const std::string topicArn = createTopicResult.body;

        // act
        const Core::HttpSocketResponse publishResult = SendPostCommand(_snsBaseUrl + "publish?topicArn=" + Core::StringUtils::UrlEncode(topicArn), Core::StringUtils::UrlEncode(TEST_MESSAGE_JSON));
        BOOST_CHECK_EQUAL(publishResult.statusCode == http::status::ok, true);
        const Database::Entity::SNS::MessageList messageList = _snsDatabase.ListMessages();

        // assert
        BOOST_CHECK_EQUAL(publishResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(1, messageList.size());
    }

    BOOST_FIXTURE_TEST_CASE(TopicDeleteTest, SNSServiceJavaTest) {

        // arrange
        const Core::HttpSocketResponse createTopicResult = SendPostCommand(_snsBaseUrl + "createTopic?name=" + Core::StringUtils::UrlEncode(TEST_TOPIC), {});
        BOOST_CHECK_EQUAL(createTopicResult.statusCode == http::status::ok, true);
        const std::string topicArn = createTopicResult.body;

        // act
        const Core::HttpSocketResponse deleteResult = SendDeleteCommand(_snsBaseUrl + "deleteTopic?topicArn=" + Core::StringUtils::UrlEncode(topicArn), {});
        const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();

        // assert
        BOOST_CHECK_EQUAL(deleteResult.statusCode == http::status::ok, true);
        BOOST_CHECK_EQUAL(0, topicList.size());
    }

}// namespace AwsMock::Service
