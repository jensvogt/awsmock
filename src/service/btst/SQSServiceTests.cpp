//
// Created by vogje01 on 02/06/2023.
//

// AwsMock includes
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/sqs/SQSService.h>

// AwsMOck includes
#include "TestBase.h"


#include <awsmock/core/TestUtils.h>
#include <awsmock/utils/SqsUtils.h>

#define BOOST_TEST_MODULE SQSServiceTests
#define REGION "eu-central-1"
#define QUEUE "test-queue"
#define QUEUE_URL "http://localhost:4566/000000000000/test-queue"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define BODY_MD5 "bf54bf4281dc11635fcdd2d7d6c9e126"
#define EMPTY_MD5 "d41d8cd98f00b204e9800998ecf8427e"
#define OWNER "test-owner"

namespace AwsMock::Service {

    /**
     * @brief Test the SQS service layer
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SQSServiceTest : TestBase {

        SQSServiceTest() {
            // ReSharper disable once CppExpressionWithoutSideEffects
            _database.DeleteAllQueues();
            // ReSharper disable once CppExpressionWithoutSideEffects
            _database.DeleteAllMessages();
        }

        Database::SQSDatabase &_database = Database::SQSDatabase::instance();
        SQSService _service;
    };

    BOOST_FIXTURE_TEST_CASE(QueueCreateTest, SQSServiceTest) {

        // arrange
        const Dto::SQS::CreateQueueRequest request = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};

        // act
        const Dto::SQS::CreateQueueResponse response = _service.CreateQueue(request);

        // assert
        BOOST_CHECK_EQUAL(response.name, QUEUE);
        BOOST_CHECK_EQUAL(response.region, REGION);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(request);
        Dto::SQS::ListQueuesRequest listQueuesRequest = {.region = REGION, .maxResults = 100, .nextToken = ""};

        // act
        Dto::SQS::ListQueuesResponse response = _service.ListQueues(listQueuesRequest);

        // assert
        BOOST_CHECK_EQUAL(response.queueList.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListEmptyTest, SQSServiceTest) {

        // arrange
        const Dto::SQS::ListQueuesRequest listQueuesRequest = {.region = REGION, .maxResults = 100, .nextToken = ""};

        // act
        const Dto::SQS::ListQueuesResponse response = _service.ListQueues(listQueuesRequest);

        // assert
        BOOST_CHECK_EQUAL(response.queueList.empty(), true);
    }

    BOOST_FIXTURE_TEST_CASE(QueueNotExistenceTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest request = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(request);
        Dto::SQS::GetQueueDetailsRequest getQueueDetailsRequest;
        getQueueDetailsRequest.region = REGION;
        getQueueDetailsRequest.queueArn = "notExist";

        // act
        BOOST_CHECK_THROW({
        try {
            Dto::SQS::GetQueueDetailsResponse response = _service.GetQueueDetails(getQueueDetailsRequest);
        } catch( const Core::ServiceException& e ) {
            BOOST_CHECK_EQUAL("Queue does not exist, queueArn: notExist", e.what());
            throw;
        } }, Core::ServiceException);
    }

    BOOST_FIXTURE_TEST_CASE(QueueGetDetailsTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::SendMessageRequest sendMessageRequest;
        sendMessageRequest.region = REGION;
        sendMessageRequest.queueUrl = queueResponse.queueUrl;
        sendMessageRequest.body = BODY;
        Dto::SQS::SendMessageResponse sendMessageResponse = _service.SendMessage(sendMessageRequest);
        Dto::SQS::GetQueueDetailsRequest getQueueDetailsRequest;
        getQueueDetailsRequest.region = REGION;
        getQueueDetailsRequest.queueArn = queueResponse.queueArn;

        // act
        Dto::SQS::GetQueueDetailsResponse response = _service.GetQueueDetails(getQueueDetailsRequest);

        // assert
        BOOST_CHECK_EQUAL(1, response.available);
        BOOST_CHECK_EQUAL(0, response.delayed);
        BOOST_CHECK_EQUAL(0, response.invisible);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListArnsTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);

        // act
        auto [queueArns] = _service.ListQueueArns();

        // assert
        BOOST_CHECK_EQUAL(1, queueArns.size());
        BOOST_CHECK_EQUAL(queueArns.at(0), queueResponse.queueArn);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListCountersTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::SendMessageRequest sendMessageRequest;
        sendMessageRequest.region = REGION;
        sendMessageRequest.queueUrl = queueResponse.queueUrl;
        sendMessageRequest.body = BODY;
        Dto::SQS::SendMessageResponse sendMessageResponse = _service.SendMessage(sendMessageRequest);
        Dto::SQS::ListQueueCountersRequest listQueueCountersRequest;
        listQueueCountersRequest.region = REGION;

        // act
        Dto::SQS::ListQueueCountersResponse response = _service.ListQueueCounters(listQueueCountersRequest);

        // assert
        BOOST_CHECK_EQUAL(1, response.total);
        BOOST_CHECK_EQUAL(1, response.queueCounters.at(0).available);
    }

    BOOST_FIXTURE_TEST_CASE(QueueListTagsTest, SQSServiceTest) {

        // arrange
        std::map<std::string, std::string> inputTags = {{"version", "1.0"}};
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER, .tags = inputTags};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        Dto::SQS::ListQueueTagsRequest listQueueTagsRequest = {.region = REGION, .queueUrl = queueResponse.queueUrl};

        // act
        auto [tags, total] = _service.ListQueueTags(listQueueTagsRequest);

        // assert
        BOOST_CHECK_EQUAL(1, total);
        BOOST_CHECK_EQUAL(1, tags.size());
        BOOST_CHECK_EQUAL(tags["version"], "1.0");
    }

    BOOST_FIXTURE_TEST_CASE(QueueDeleteTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .owner = OWNER};
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);

        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;
        Dto::SQS::DeleteQueueRequest deleteRequest = {.region = REGION, .queueUrl = queueUrl};
        deleteRequest.requestId = Core::StringUtils::CreateRandomUuid();

        // act
        BOOST_CHECK_NO_THROW({ Dto::SQS::DeleteQueueResponse response = _service.DeleteQueue(deleteRequest); });

        // assert
        BOOST_CHECK_EQUAL(0, _database.ListQueues(REGION).size());
    }

    BOOST_FIXTURE_TEST_CASE(MessageCreateTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER};
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();

        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest request;
        request.queueUrl = queueUrl;
        request.body = BODY;
        request.region = REGION;
        request.requestId = Core::StringUtils::CreateRandomUuid();

        // act
        Dto::SQS::SendMessageResponse response = _service.SendMessage(request);

        // assert
        BOOST_CHECK_EQUAL(response.messageId.empty(), false);
        BOOST_CHECK_EQUAL(response.md5Body, BODY_MD5);
        BOOST_CHECK_EQUAL(response.md5MessageAttributes, EMPTY_MD5);
        BOOST_CHECK_EQUAL(response.md5MessageSystemAttributes, EMPTY_MD5);
    }

    BOOST_FIXTURE_TEST_CASE(MessagesCreateTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER};
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();

        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest request1;
        request1.region = REGION;
        request1.queueUrl = queueUrl;
        request1.body = BODY;
        request1.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::SendMessageRequest request2;
        request2.region = REGION;
        request2.queueUrl = queueUrl;
        request2.body = BODY;
        request2.requestId = Core::StringUtils::CreateRandomUuid();

        // act
        Dto::SQS::SendMessageResponse response1 = _service.SendMessage(request1);
        Dto::SQS::SendMessageResponse response2 = _service.SendMessage(request2);

        // assert
        BOOST_CHECK_EQUAL(response1.messageId.empty(), false);
        BOOST_CHECK_EQUAL(response1.md5Body, BODY_MD5);
        BOOST_CHECK_EQUAL(response1.md5MessageAttributes, EMPTY_MD5);
        BOOST_CHECK_EQUAL(response1.md5MessageSystemAttributes, EMPTY_MD5);
        BOOST_CHECK_EQUAL(response2.messageId.empty(), false);
        BOOST_CHECK_EQUAL(response2.md5Body, BODY_MD5);
        BOOST_CHECK_EQUAL(response2.md5MessageAttributes, EMPTY_MD5);
        BOOST_CHECK_EQUAL(response2.md5MessageSystemAttributes, EMPTY_MD5);
    }

    BOOST_FIXTURE_TEST_CASE(MessageReceiveTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest msgRequest;
        msgRequest.region = REGION;
        msgRequest.queueUrl = queueUrl;
        msgRequest.body = BODY;
        Dto::SQS::SendMessageResponse msgResponse = _service.SendMessage(msgRequest);

        // act
        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = REGION;
        receiveRequest.queueUrl = queueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 1;
        Dto::SQS::ReceiveMessageResponse receiveResponse = _service.ReceiveMessages(receiveRequest);

        // assert
        BOOST_CHECK_EQUAL(receiveResponse.messageList.size(), 1);
    }

    BOOST_FIXTURE_TEST_CASE(MessageDeleteTest, SQSServiceTest) {

        // arrange
        Dto::SQS::CreateQueueRequest queueRequest = {.region = REGION, .queueName = QUEUE, .queueUrl = QUEUE_URL, .owner = OWNER, .requestId = Core::StringUtils::CreateRandomUuid()};
        Dto::SQS::CreateQueueResponse queueResponse = _service.CreateQueue(queueRequest);
        std::string queueUrl = _service.GetQueueUrl({.region = REGION, .queueName = QUEUE}).queueUrl;

        Dto::SQS::SendMessageRequest msgRequest;
        msgRequest.region = REGION;
        msgRequest.queueUrl = queueUrl;
        msgRequest.body = BODY;
        Dto::SQS::SendMessageResponse msgResponse = _service.SendMessage(msgRequest);

        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = REGION;
        receiveRequest.queueUrl = queueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 1;
        Dto::SQS::ReceiveMessageResponse receiveResponse = _service.ReceiveMessages(receiveRequest);

        // act
        Dto::SQS::DeleteMessageRequest delRequest;
        delRequest.queueUrl = queueUrl;
        delRequest.receiptHandle = receiveResponse.messageList[0].receiptHandle;
        Dto::SQS::DeleteMessageResponse delResponse;
        BOOST_CHECK_NO_THROW({ _service.DeleteMessage(delRequest); });

        // assert
        BOOST_CHECK_EQUAL(0, _database.CountMessages(Core::AwsUtils::ConvertSQSQueueUrlToArn(REGION, queueUrl)));
    }

    BOOST_FIXTURE_TEST_CASE(GetMd5AttributesTest, SQSServiceTest) {

        // arrange
        //
        // MessageAttribute.1.Name=contentType
        // MessageAttribute.1.Value.StringValue=application/json
        // MessageAttribute.1.Value.DataType=String
        //
        Database::Entity::SQS::MessageAttribute messageAttribute;
        messageAttribute.stringValue = "application/json";
        messageAttribute.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        std::map<std::string, Database::Entity::SQS::MessageAttribute> messageAttributes;
        messageAttributes["contentType"] = messageAttribute;

        // act
        const std::string md5sum = Database::SqsUtils::CreateMd5OfMessageAttributes(messageAttributes);

        // assert
        BOOST_CHECK_EQUAL("6ed5f16969b625c8d900cbd5da557e9e", md5sum);
    }

    BOOST_FIXTURE_TEST_CASE(GetMd5AttributeListTest, SQSServiceTest) {

        // arrange
        //
        // MessageAttribute.1.Name=contentType
        // MessageAttribute.1.Value.StringValue=application/json
        // MessageAttribute.1.Value.DataType=String
        // MessageAttribute.2.Name=contentLength
        // MessageAttribute.2.Value.StringValue=42
        // MessageAttribute.2.Value.DataType=Number
        //
        Database::Entity::SQS::MessageAttribute messageAttribute1;
        const std::string name1 = "contentType";
        messageAttribute1.stringValue = "application/json";
        messageAttribute1.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        Database::Entity::SQS::MessageAttribute messageAttribute2;
        const std::string name2 = "contentType";
        messageAttribute2.stringValue = "42";
        messageAttribute2.dataType = Database::Entity::SQS::MessageAttributeType::STRING;
        std::map<std::string, Database::Entity::SQS::MessageAttribute> messageAttributes;
        messageAttributes[name1] = messageAttribute1;
        messageAttributes[name2] = messageAttribute2;

        // act
        const std::string md5sum = Database::SqsUtils::CreateMd5OfMessageAttributes(messageAttributes);

        // assert
        BOOST_CHECK_EQUAL("e555c73571247ca6f0c0078e86da8c96", md5sum);
    }

}// namespace AwsMock::Service
