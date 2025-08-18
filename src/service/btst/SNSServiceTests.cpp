//
// Created by vogje01 on 02/06/2023.
//

// AwsMock includes

#include "TestBase.h"
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/sns/SNSService.h>
#include <awsmock/service/sqs/SQSService.h>

#define BOOST_TEST_MODULE SNSServiceTests
#define REGION "eu-central-1"
#define TOPIC "test-topic"
#define QUEUE "test-queue"
#define QUEUE_URL "http://localhost:4566/000000000000/test-queue"
#define BODY "{\"TestObject\": \"TestValue\"}"
#define BODY_MD5 "bf54bf4281dc11635fcdd2d7d6c9e126"
#define OWNER "test-owner"

namespace AwsMock::Service {

    /**
     * @brief Test the SNS service layer
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSServiceTest : TestBase {

      protected:

        SNSServiceTest() {
            // ReSharper disable once CppExpressionWithoutSideEffects
            _snsDatabase.DeleteAllMessages();
            // ReSharper disable once CppExpressionWithoutSideEffects
            _snsDatabase.DeleteAllTopics();
            // ReSharper disable once CppExpressionWithoutSideEffects
            _sqsDatabase.DeleteAllMessages();
            // ReSharper disable once CppExpressionWithoutSideEffects
            _sqsDatabase.DeleteAllQueues();
        }

        boost::asio::io_context _ioContext;
        Database::SNSDatabase &_snsDatabase = Database::SNSDatabase::instance();
        Database::SQSDatabase &_sqsDatabase = Database::SQSDatabase::instance();
        SNSService _snsService{_ioContext};
        SQSService _sqsService{_ioContext};
    };

    BOOST_FIXTURE_TEST_CASE(TopicCreateTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;

        // act
        const Dto::SNS::CreateTopicResponse response = _snsService.CreateTopic(createRequest);

        // assert
        BOOST_CHECK_EQUAL(response.region, REGION);
        BOOST_CHECK_EQUAL(response.topicName, TOPIC);
    }

    BOOST_FIXTURE_TEST_CASE(TopicListTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        const Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);

        // act
        const Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);

        // assert
        BOOST_CHECK_EQUAL(1, response.topics.size());
    }

    BOOST_FIXTURE_TEST_CASE(TopicPurgeTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        const Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);
        Dto::SNS::PublishRequest request;
        request.region = REGION;
        request.topicArn = topicResponse.topicArn;
        request.message = BODY;
        Dto::SNS::PublishResponse response = _snsService.Publish(request);
        BOOST_CHECK_EQUAL(response.messageId.length() > 1, true);
        Dto::SNS::PurgeTopicRequest purgeRequest;
        purgeRequest.topicArn = topicResponse.topicArn;

        // act
        long count = _snsService.PurgeTopic(purgeRequest);

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_FIXTURE_TEST_CASE(TopicDeleteTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        const Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);

        // act
        BOOST_CHECK_NO_THROW({
            Dto::SNS::DeleteTopicResponse response = _snsService.DeleteTopic(topicResponse.region, topicResponse.topicArn);
            BOOST_CHECK_EQUAL(response.requestId.empty(), false);
        });

        // assert
        BOOST_CHECK_EQUAL(0, _snsDatabase.ListTopics(REGION).size());
    }

    BOOST_FIXTURE_TEST_CASE(SubscribeTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);
        Dto::SQS::CreateQueueRequest queueRequest;
        queueRequest.queueName = QUEUE;
        queueRequest.queueUrl = QUEUE_URL;
        queueRequest.owner = OWNER;
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);

        // act
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);

        // assert
        BOOST_CHECK_EQUAL(subscribeResponse.subscriptionArn.empty(), false);
        BOOST_CHECK_EQUAL(48, response.topics[0].size());
    }
    BOOST_FIXTURE_TEST_CASE(SubscriptionUpdateTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);
        Dto::SQS::CreateQueueRequest queueRequest;
        queueRequest.queueName = QUEUE;
        queueRequest.queueUrl = QUEUE_URL;
        queueRequest.owner = OWNER;
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);
        Dto::SNS::UpdateSubscriptionRequest updateRequest;
        updateRequest.topicArn = topicResponse.topicArn;
        updateRequest.subscriptionArn = subscribeResponse.subscriptionArn;
        updateRequest.protocol = "SQS";
        updateRequest.endpoint = "foobar";
        updateRequest.owner = "bar";

        // act
        Dto::SNS::UpdateSubscriptionResponse result = _snsService.UpdateSubscription(updateRequest);

        // assert
        BOOST_CHECK_EQUAL(result.subscriptionArn.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(SubscriptionListTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);
        Dto::SQS::CreateQueueRequest queueRequest;
        queueRequest.queueName = QUEUE;
        queueRequest.queueUrl = QUEUE_URL;
        queueRequest.owner = OWNER;
        queueRequest.region = REGION;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();
        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::ListSubscriptionsByTopicRequest listRequest;
        listRequest.region = REGION;
        listRequest.topicArn = topicResponse.topicArn;

        // act
        Dto::SNS::ListSubscriptionsByTopicResponse listResponse = _snsService.ListSubscriptionsByTopic(listRequest);

        // assert
        BOOST_CHECK_EQUAL(listResponse.subscriptions.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(UnsubscribeTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);
        Dto::SQS::CreateQueueRequest queueRequest;
        queueRequest.region = REGION;
        queueRequest.queueName = QUEUE;
        queueRequest.queueUrl = QUEUE_URL;
        queueRequest.owner = OWNER;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();

        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);
        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);

        // act
        Dto::SNS::UnsubscribeRequest unsubscribeRequest;
        unsubscribeRequest.region = REGION;
        unsubscribeRequest.subscriptionArn = subscribeResponse.subscriptionArn;
        Dto::SNS::UnsubscribeResponse unsubscribeResponse = _snsService.Unsubscribe(unsubscribeRequest);
        Dto::SNS::ListTopicsResponse response = _snsService.ListTopics(REGION);

        // assert
        BOOST_CHECK_EQUAL(subscribeResponse.subscriptionArn.empty(), false);
        BOOST_CHECK_EQUAL(48, response.topics[0].size());
    }

    BOOST_FIXTURE_TEST_CASE(PublishMessageTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);
        Dto::SNS::PublishRequest request;
        request.region = REGION;
        request.topicArn = topicResponse.topicArn;
        request.message = BODY;

        // act
        Dto::SNS::PublishResponse response = _snsService.Publish(request);

        // assert
        BOOST_CHECK_EQUAL(response.messageId.empty(), false);
        BOOST_CHECK_EQUAL(response.ToXml().empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(MessageReceiveTest, SNSServiceTest) {

        // arrange
        Dto::SNS::CreateTopicRequest createRequest;
        createRequest.region = REGION;
        createRequest.topicName = TOPIC;
        createRequest.owner = OWNER;
        Dto::SNS::CreateTopicResponse topicResponse = _snsService.CreateTopic(createRequest);
        Dto::SQS::CreateQueueRequest queueRequest;
        queueRequest.region = REGION;
        queueRequest.queueName = QUEUE;
        queueRequest.queueUrl = QUEUE_URL;
        queueRequest.owner = OWNER;
        queueRequest.requestId = Core::StringUtils::CreateRandomUuid();

        Dto::SQS::CreateQueueResponse queueResponse = _sqsService.CreateQueue(queueRequest);

        Dto::SQS::GetQueueUrlRequest getQueueUrlRequest;
        getQueueUrlRequest.region = REGION;
        getQueueUrlRequest.queueName = QUEUE;
        std::string queueUrl = _sqsService.GetQueueUrl(getQueueUrlRequest).queueUrl;
        const std::string &resultQueueUrl = queueUrl;

        Dto::SNS::SubscribeRequest subscribeRequest;
        subscribeRequest.region = REGION;
        subscribeRequest.topicArn = topicResponse.topicArn;
        subscribeRequest.protocol = "sqs";
        subscribeRequest.endpoint = queueResponse.queueArn;
        Dto::SNS::SubscribeResponse subscribeResponse = _snsService.Subscribe(subscribeRequest);
        Dto::SNS::PublishRequest request;
        request.region = REGION;
        request.topicArn = topicResponse.topicArn;
        request.message = BODY;
        Dto::SNS::PublishResponse response = _snsService.Publish(request);

        // act
        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = REGION;
        receiveRequest.queueUrl = resultQueueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 5;
        Dto::SQS::ReceiveMessageResponse receiveResponse = _sqsService.ReceiveMessages(receiveRequest);

        // assert
        BOOST_CHECK_EQUAL(receiveResponse.messageList.size(), 1);
    }

}// namespace AwsMock::Service
