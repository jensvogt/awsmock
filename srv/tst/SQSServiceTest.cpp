//
// Created by vogje01 on 30/05/2023.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/EventBus.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/service/sqs/SQSService.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_QUEUE "test-queue"
#define TEST_REGION "eu-central-1"
#define TEST_MESSAGE_BODY "Hello, SQS!"
#define TEST_LAMBDA_FUNCTION "test-lambda-trigger"

namespace Awsmock::Database {

    Dto::SQS::CreateQueueResponse CreateDefaultQueue(const Service::SQSService &sqsService) {
        Dto::SQS::CreateQueueRequest request;
        request.region = TEST_REGION;
        request.queueName = TEST_QUEUE;
        return sqsService.CreateQueue(request);
    }

    Dto::SQS::SendMessageResponse SendDefaultMessage(const Service::SQSService &sqsService, const std::string &queueUrl) {
        Dto::SQS::SendMessageRequest request;
        request.region = TEST_REGION;
        request.queueUrl = queueUrl;
        request.body = TEST_MESSAGE_BODY;
        return sqsService.SendMessage(request);
    }

    // Registers a lambda with an SQS event source mapping on queueArn, so SendMessage triggers
    // the synchronous invocation path in SQSService::CheckLambdaNotifications.
    void CreateLambdaWithSqsEventSource(const std::string &queueArn) {
        Entity::Lambda::Lambda lambda;
        lambda.region = TEST_REGION;
        lambda.function = TEST_LAMBDA_FUNCTION;
        lambda.runtime = "java21";
        lambda.arn = Core::AwsUtils::CreateLambdaArn(TEST_REGION, TEST_ACCOUNT_ID, TEST_LAMBDA_FUNCTION);
        lambda.role = "arn:aws:iam::000000000000:role/ignoreme";
        lambda.handler = "org.springframework.cloud.function.adapter.aws.FunctionInvoker";

        Entity::Lambda::EventSourceMapping eventSource;
        eventSource.eventSourceArn = queueArn;
        eventSource.type = "SQS";
        eventSource.enabled = true;
        eventSource.uuid = Core::StringUtils::CreateRandomUuid();
        lambda.eventSources.emplace_back(eventSource);

        RepositoryFactory::instance().lambdaRepository()->createLambda(lambda);
    }

    struct SQSServiceFixture {
        SQSServiceFixture() = default;
        ~SQSServiceFixture() {
            try {
                const long deletedQueues = RepositoryFactory::instance().sqsRepository()->deleteAllQueues();
                log_debug << "Queues deleted, count: " << deletedQueues;
                const long deletedMessages = RepositoryFactory::instance().sqsRepository()->deleteAllMessages();
                log_debug << "Messages deleted, count: " << deletedMessages;
                const long deletedLambdas = RepositoryFactory::instance().lambdaRepository()->deleteAllLambdas();
                log_debug << "Lambdas deleted, count: " << deletedLambdas;
            } catch (const std::exception &exc) {
                log_error << "SQS fixture cleanup failed: " << exc.what();
            }
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SQSServiceTests, SQSServiceFixture)

    BOOST_AUTO_TEST_CASE(CreateQueueTest) {

        // arrange
        const Service::SQSService sqsService;
        Dto::SQS::CreateQueueRequest request;
        request.region = TEST_REGION;
        request.queueName = TEST_QUEUE;

        // act
        const Dto::SQS::CreateQueueResponse response = sqsService.CreateQueue(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.queueUrl.empty());
        BOOST_CHECK_EQUAL(false, response.queueArn.empty());
        BOOST_CHECK_EQUAL(TEST_QUEUE, response.queueName);
    }

    BOOST_AUTO_TEST_CASE(ListQueuesTest) {

        // arrange
        const Service::SQSService sqsService;
        Dto::SQS::CreateQueueRequest createRequest1;
        createRequest1.region = TEST_REGION;
        createRequest1.queueName = "test-queue-1";
        Dto::SQS::CreateQueueResponse createResponse1 = sqsService.CreateQueue(createRequest1);
        BOOST_CHECK_EQUAL(false, createResponse1.queueArn.empty());
        BOOST_CHECK_EQUAL("test-queue-1", createRequest1.queueName);

        Dto::SQS::CreateQueueRequest createRequest2;
        createRequest2.region = TEST_REGION;
        createRequest2.queueName = "test-queue-2";
        Dto::SQS::CreateQueueResponse createResponse2 = sqsService.CreateQueue(createRequest2);
        BOOST_CHECK_EQUAL(false, createResponse2.queueArn.empty());
        BOOST_CHECK_EQUAL("test-queue-2", createRequest2.queueName);

        Dto::SQS::ListQueuesRequest listRequest;
        listRequest.region = TEST_REGION;

        // act
        const Dto::SQS::ListQueuesResponse response = sqsService.ListQueues(listRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.queueUrls.empty());
        BOOST_CHECK_EQUAL(2, response.total);
    }

    BOOST_AUTO_TEST_CASE(GetQueueUrlTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);
        BOOST_CHECK_EQUAL(false, createResponse.queueUrl.empty());

        Dto::SQS::GetQueueUrlRequest request;
        request.region = TEST_REGION;
        request.queueName = TEST_QUEUE;

        // act
        const Dto::SQS::GetQueueUrlResponse response = sqsService.GetQueueUrl(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.queueUrl.empty());
        BOOST_CHECK_EQUAL(createResponse.queueUrl, response.queueUrl);
    }

    BOOST_AUTO_TEST_CASE(GetQueueAttributesTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);

        Dto::SQS::GetQueueAttributesRequest request;
        request.region = TEST_REGION;
        request.queueUrl = createResponse.queueUrl;
        request.attributeNames = {"All"};

        // act
        const Dto::SQS::GetQueueAttributesResponse response = sqsService.GetQueueAttributes(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.attributes.empty());
    }

    BOOST_AUTO_TEST_CASE(SendMessageTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);

        Dto::SQS::SendMessageRequest request;
        request.region = TEST_REGION;
        request.queueUrl = createResponse.queueUrl;
        request.body = TEST_MESSAGE_BODY;

        // act
        const Dto::SQS::SendMessageResponse response = sqsService.SendMessage(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.messageId.empty());
        BOOST_CHECK_EQUAL(false, response.md5Body.empty());
    }

    BOOST_AUTO_TEST_CASE(SendMessageLambdaTriggerSuccessTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);
        CreateLambdaWithSqsEventSource(createResponse.queueArn);

        // Simulate a successful synchronous lambda invocation, as LambdaController would do.
        const boost::signals2::scoped_connection connection = Core::EventBus::instance().sigLambdaInvoke.connect(
                [](const std::string &, const std::string &, const std::string &, const std::string &,
                   const std::shared_ptr<std::promise<std::pair<int, std::string>>> &promise) {
                    if (promise) {
                        promise->set_value({200, "\"OK\""});
                    }
                });

        Dto::SQS::SendMessageRequest request;
        request.region = TEST_REGION;
        request.queueUrl = createResponse.queueUrl;
        request.body = TEST_MESSAGE_BODY;

        // act
        const Dto::SQS::SendMessageResponse response = sqsService.SendMessage(request);

        // assert: the lambda invocation succeeded, so the message must be removed from the queue
        BOOST_CHECK_EQUAL(false, response.messageId.empty());
        BOOST_CHECK_EQUAL(false, RepositoryFactory::instance().sqsRepository()->messageExistsByMessageId(response.messageId));
    }

    BOOST_AUTO_TEST_CASE(SendMessageLambdaTriggerFailureTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);
        CreateLambdaWithSqsEventSource(createResponse.queueArn);

        // Simulate a failed synchronous lambda invocation (unhandled exception in the function body).
        const boost::signals2::scoped_connection connection = Core::EventBus::instance().sigLambdaInvoke.connect(
                [](const std::string &, const std::string &, const std::string &, const std::string &,
                   const std::shared_ptr<std::promise<std::pair<int, std::string>>> &promise) {
                    if (promise) {
                        promise->set_value({200, R"({"errorType":"Exception","errorMessage":"boom"})"});
                    }
                });

        Dto::SQS::SendMessageRequest request;
        request.region = TEST_REGION;
        request.queueUrl = createResponse.queueUrl;
        request.body = TEST_MESSAGE_BODY;

        // act
        const Dto::SQS::SendMessageResponse response = sqsService.SendMessage(request);

        // assert: the lambda invocation failed, so the message must stay in the queue for a retry
        BOOST_CHECK_EQUAL(false, response.messageId.empty());
        BOOST_CHECK_EQUAL(true, RepositoryFactory::instance().sqsRepository()->messageExistsByMessageId(response.messageId));
    }

    BOOST_AUTO_TEST_CASE(ReceiveMessageTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);
        SendDefaultMessage(sqsService, createResponse.queueUrl);

        Dto::SQS::ReceiveMessageRequest request;
        request.region = TEST_REGION;
        request.queueUrl = createResponse.queueUrl;
        request.maxMessages = 10;
        request.waitTimeSeconds = 0;

        // act
        const Dto::SQS::ReceiveMessageResponse response = sqsService.ReceiveMessages(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.messageList.empty());
        BOOST_CHECK_EQUAL(1, response.messageList.size());
        BOOST_CHECK_EQUAL(TEST_MESSAGE_BODY, response.messageList.at(0).body);
    }

    BOOST_AUTO_TEST_CASE(DeleteMessageTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);
        SendDefaultMessage(sqsService, createResponse.queueUrl);

        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = TEST_REGION;
        receiveRequest.queueUrl = createResponse.queueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 0;
        const Dto::SQS::ReceiveMessageResponse receiveResponse = sqsService.ReceiveMessages(receiveRequest);
        BOOST_CHECK_EQUAL(false, receiveResponse.messageList.empty());

        Dto::SQS::DeleteMessageRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.queueUrl = createResponse.queueUrl;
        deleteRequest.receiptHandle = receiveResponse.messageList.at(0).receiptHandle;

        // act / assert (no exception thrown means success)
        BOOST_CHECK_NO_THROW(sqsService.DeleteMessage(deleteRequest));

        // verify message is gone
        const Dto::SQS::ReceiveMessageResponse afterDelete = sqsService.ReceiveMessages(receiveRequest);
        BOOST_CHECK_EQUAL(true, afterDelete.messageList.empty());
    }

    BOOST_AUTO_TEST_CASE(SendMessageBatchTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);

        Dto::SQS::MessageEntry entry1;
        entry1.id = "1";
        entry1.body = "Message 1";

        Dto::SQS::MessageEntry entry2;
        entry2.id = "2";
        entry2.body = "Message 2";

        Dto::SQS::SendMessageBatchRequest request;
        request.region = TEST_REGION;
        request.queueUrl = createResponse.queueUrl;
        request.entries = {entry1, entry2};

        // act
        const Dto::SQS::SendMessageBatchResponse response = sqsService.SendMessageBatch(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.successful.empty());
        BOOST_CHECK_EQUAL(2, response.successful.size());
        BOOST_CHECK_EQUAL(true, response.failed.empty());
    }

    BOOST_AUTO_TEST_CASE(PurgeQueueTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);
        SendDefaultMessage(sqsService, createResponse.queueUrl);
        SendDefaultMessage(sqsService, createResponse.queueUrl);

        Dto::SQS::PurgeQueueRequest purgeRequest;
        purgeRequest.region = TEST_REGION;
        purgeRequest.queueUrl = createResponse.queueUrl;

        // act
        const long deleted = sqsService.PurgeQueue(purgeRequest);

        // assert
        BOOST_CHECK_EQUAL(2, deleted);

        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = TEST_REGION;
        receiveRequest.queueUrl = createResponse.queueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 0;
        const Dto::SQS::ReceiveMessageResponse receiveResponse = sqsService.ReceiveMessages(receiveRequest);
        BOOST_CHECK_EQUAL(true, receiveResponse.messageList.empty());
    }

    BOOST_AUTO_TEST_CASE(DeleteQueueTest) {

        // arrange
        const Service::SQSService sqsService;
        const Dto::SQS::CreateQueueResponse createResponse = CreateDefaultQueue(sqsService);
        BOOST_CHECK_EQUAL(false, createResponse.queueUrl.empty());

        Dto::SQS::DeleteQueueRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.queueUrl = createResponse.queueUrl;

        // act
        const Dto::SQS::DeleteQueueResponse response = sqsService.DeleteQueue(deleteRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.queueUrl.empty());

        // verify queue is gone
        Dto::SQS::ListQueuesRequest listRequest;
        listRequest.region = TEST_REGION;
        const Dto::SQS::ListQueuesResponse listResponse = sqsService.ListQueues(listRequest);
        BOOST_CHECK_EQUAL(0, listResponse.total);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
