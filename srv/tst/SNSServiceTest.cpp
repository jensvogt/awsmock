//
// Created by vogje01 on 02/06/2023.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <../../db/include/awsmock/repository/sns/SNSMongoRepository.h>
#include <../../db/include/awsmock/repository/sqs/SQSMongoRepository.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/service/sns/SNSService.h>
#include <awsmock/service/sqs/SQSService.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_TOPIC "test-topic"
#define TEST_QUEUE "test-queue"
#define TEST_BODY "{\"TestObject\": \"TestValue\"}"
#define TEST_OWNER "test-owner"

namespace Awsmock::Database {

    Dto::SNS::CreateTopicResponse CreateDefaultTopic(const Service::SNSService &snsService) {
        Dto::SNS::CreateTopicRequest request;
        request.region = TEST_REGION;
        request.topicName = TEST_TOPIC;
        request.owner = TEST_OWNER;
        return snsService.CreateTopic(request);
    }

    Dto::SQS::CreateQueueResponse CreateDefaultSubscriptionQueue(const Service::SQSService &sqsService) {
        Dto::SQS::CreateQueueRequest request;
        request.region = TEST_REGION;
        request.queueName = TEST_QUEUE;
        return sqsService.CreateQueue(request);
    }

    Dto::SNS::SubscribeResponse SubscribeQueueToTopic(const Service::SNSService &snsService, const std::string &topicArn, const std::string &queueArn) {
        Dto::SNS::SubscribeRequest request;
        request.region = TEST_REGION;
        request.topicArn = topicArn;
        request.protocol = "sqs";
        request.endpoint = queueArn;
        return snsService.Subscribe(request);
    }

    struct SNSServiceFixture {
        SNSServiceFixture() = default;
        ~SNSServiceFixture() {
            try {
                const long deletedMessages = SNSMongoRepository::instance().DeleteAllMessages();
                log_debug << "SNS messages deleted, count: " << deletedMessages;
                const long deletedTopics = SNSMongoRepository::instance().DeleteAllTopics();
                log_debug << "SNS topics deleted, count: " << deletedTopics;
                const long deletedSqsMessages = SQSMongoRepository::instance().DeleteAllMessages();
                log_debug << "SQS messages deleted, count: " << deletedSqsMessages;
                const long deletedQueues = SQSMongoRepository::instance().DeleteAllQueues();
                log_debug << "SQS queues deleted, count: " << deletedQueues;
            } catch (const std::exception &exc) {
                log_error << "SNS fixture cleanup failed: " << exc.what();
            }
        }
    };

    BOOST_FIXTURE_TEST_SUITE(SNSServiceTests, SNSServiceFixture)

    BOOST_AUTO_TEST_CASE(TopicCreateTest) {

        // arrange
        const Service::SNSService snsService;
        Dto::SNS::CreateTopicRequest request;
        request.region = TEST_REGION;
        request.topicName = TEST_TOPIC;
        request.owner = TEST_OWNER;

        // act
        const Dto::SNS::CreateTopicResponse response = snsService.CreateTopic(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_REGION, response.region);
        BOOST_CHECK_EQUAL(TEST_TOPIC, response.topicName);
        BOOST_CHECK_EQUAL(false, response.topicArn.empty());
    }

    BOOST_AUTO_TEST_CASE(TopicListTest) {

        // arrange
        const Service::SNSService snsService;
        CreateDefaultTopic(snsService);

        // act
        const Dto::SNS::ListTopicsResponse response = snsService.ListTopics(TEST_REGION);

        // assert
        BOOST_CHECK_EQUAL(1, response.topics.size());
        BOOST_CHECK_EQUAL(false, response.topics.at(0).empty());
    }

    BOOST_AUTO_TEST_CASE(TopicDeleteTest) {

        // arrange
        const Service::SNSService snsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);

        Dto::SNS::DeleteTopicRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.topicArn = topicResponse.topicArn;

        // act
        const Dto::SNS::DeleteTopicResponse deleteResponse = snsService.DeleteTopic(deleteRequest);

        // assert
        BOOST_CHECK_EQUAL(false, deleteResponse.requestId.empty());
        BOOST_CHECK_EQUAL(0, SNSMongoRepository::instance().ListTopics(TEST_REGION).size());
    }

    BOOST_AUTO_TEST_CASE(TopicPurgeTest) {

        // arrange
        const Service::SNSService snsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);

        Dto::SNS::PublishRequest publishRequest;
        publishRequest.region = TEST_REGION;
        publishRequest.topicArn = topicResponse.topicArn;
        publishRequest.message = TEST_BODY;
        Dto::SNS::PublishResponse publishResponse = snsService.Publish(publishRequest);
        BOOST_CHECK_EQUAL(false, publishResponse.messageId.empty());

        Dto::SNS::PurgeTopicRequest purgeRequest;
        purgeRequest.topicArn = topicResponse.topicArn;

        // act
        const long count = snsService.PurgeTopic(purgeRequest);

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_AUTO_TEST_CASE(PublishMessageTest) {

        // arrange
        const Service::SNSService snsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);

        Dto::SNS::PublishRequest request;
        request.region = TEST_REGION;
        request.topicArn = topicResponse.topicArn;
        request.message = TEST_BODY;

        // act
        const Dto::SNS::PublishResponse response = snsService.Publish(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.messageId.empty());
        BOOST_CHECK_EQUAL(false, response.ToXml().empty());
    }

    BOOST_AUTO_TEST_CASE(SubscribeTest) {

        // arrange
        const Service::SNSService snsService;
        const Service::SQSService sqsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);
        const Dto::SQS::CreateQueueResponse queueResponse = CreateDefaultSubscriptionQueue(sqsService);

        // act
        const Dto::SNS::SubscribeResponse subscribeResponse = SubscribeQueueToTopic(snsService, topicResponse.topicArn, queueResponse.queueArn);

        // assert
        BOOST_CHECK_EQUAL(false, subscribeResponse.subscriptionArn.empty());
    }

    BOOST_AUTO_TEST_CASE(UnsubscribeTest) {

        // arrange
        const Service::SNSService snsService;
        const Service::SQSService sqsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);
        const Dto::SQS::CreateQueueResponse queueResponse = CreateDefaultSubscriptionQueue(sqsService);
        const Dto::SNS::SubscribeResponse subscribeResponse = SubscribeQueueToTopic(snsService, topicResponse.topicArn, queueResponse.queueArn);
        BOOST_CHECK_EQUAL(false, subscribeResponse.subscriptionArn.empty());

        Dto::SNS::UnsubscribeRequest unsubscribeRequest;
        unsubscribeRequest.region = TEST_REGION;
        unsubscribeRequest.subscriptionArn = subscribeResponse.subscriptionArn;

        // act
        const Dto::SNS::UnsubscribeResponse unsubscribeResponse = snsService.Unsubscribe(unsubscribeRequest);

        // assert
        BOOST_CHECK_EQUAL(false, unsubscribeResponse.requestId.empty());
        Dto::SNS::ListSubscriptionsByTopicRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.topicArn = topicResponse.topicArn;
        const Dto::SNS::ListSubscriptionsByTopicResponse listResponse = snsService.ListSubscriptionsByTopic(listRequest);
        BOOST_CHECK_EQUAL(true, listResponse.subscriptions.empty());
    }

    BOOST_AUTO_TEST_CASE(SubscriptionUpdateTest) {

        // arrange
        const Service::SNSService snsService;
        const Service::SQSService sqsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);
        const Dto::SQS::CreateQueueResponse queueResponse = CreateDefaultSubscriptionQueue(sqsService);
        const Dto::SNS::SubscribeResponse subscribeResponse = SubscribeQueueToTopic(snsService, topicResponse.topicArn, queueResponse.queueArn);

        Dto::SNS::UpdateSubscriptionRequest updateRequest;
        updateRequest.topicArn = topicResponse.topicArn;
        updateRequest.subscriptionArn = subscribeResponse.subscriptionArn;
        updateRequest.protocol = "sqs";
        updateRequest.endpoint = queueResponse.queueArn;
        updateRequest.owner = TEST_OWNER;

        // act
        const Dto::SNS::UpdateSubscriptionResponse updateResponse = snsService.UpdateSubscription(updateRequest);

        // assert
        BOOST_CHECK_EQUAL(false, updateResponse.subscriptionArn.empty());
    }

    BOOST_AUTO_TEST_CASE(SubscriptionListTest) {

        // arrange
        const Service::SNSService snsService;
        const Service::SQSService sqsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);
        const Dto::SQS::CreateQueueResponse queueResponse = CreateDefaultSubscriptionQueue(sqsService);
        SubscribeQueueToTopic(snsService, topicResponse.topicArn, queueResponse.queueArn);

        Dto::SNS::ListSubscriptionsByTopicRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.topicArn = topicResponse.topicArn;

        // act
        const Dto::SNS::ListSubscriptionsByTopicResponse listResponse = snsService.ListSubscriptionsByTopic(listRequest);

        // assert
        BOOST_CHECK_EQUAL(false, listResponse.subscriptions.empty());
        BOOST_CHECK_EQUAL(1, listResponse.subscriptions.size());
    }

    BOOST_AUTO_TEST_CASE(TagResourceTest) {

        // arrange
        const Service::SNSService snsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);

        Dto::SNS::TagResourceRequest tagRequest;
        tagRequest.region = TEST_REGION;
        tagRequest.resourceArn = topicResponse.topicArn;
        tagRequest.tags["env"] = "test";
        tagRequest.tags["owner"] = TEST_OWNER;

        // act
        const Dto::SNS::TagResourceResponse tagResponse = snsService.TagResource(tagRequest);

        // assert
        BOOST_CHECK_EQUAL(false, tagResponse.requestId.empty());
    }

    BOOST_AUTO_TEST_CASE(MessageReceiveTest) {

        // arrange
        const Service::SNSService snsService;
        const Service::SQSService sqsService;
        const Dto::SNS::CreateTopicResponse topicResponse = CreateDefaultTopic(snsService);
        const Dto::SQS::CreateQueueResponse queueResponse = CreateDefaultSubscriptionQueue(sqsService);
        SubscribeQueueToTopic(snsService, topicResponse.topicArn, queueResponse.queueArn);

        Dto::SNS::PublishRequest publishRequest;
        publishRequest.region = TEST_REGION;
        publishRequest.topicArn = topicResponse.topicArn;
        publishRequest.message = TEST_BODY;
        const Dto::SNS::SubscribeResponse subscribeResponse = SubscribeQueueToTopic(snsService, topicResponse.topicArn, queueResponse.queueArn);
        BOOST_CHECK_EQUAL(false, subscribeResponse.subscriptionArn.empty());

        Dto::SNS::PublishRequest request;
        request.region = TEST_REGION;
        request.topicArn = topicResponse.topicArn;
        request.message = TEST_BODY;
        const Dto::SNS::PublishResponse publishResponse = snsService.Publish(request);
        BOOST_CHECK_EQUAL(false, publishResponse.messageId.empty());

        Dto::SQS::GetQueueUrlRequest getUrlRequest;
        getUrlRequest.region = TEST_REGION;
        getUrlRequest.queueName = TEST_QUEUE;
        const std::string queueUrl = sqsService.GetQueueUrl(getUrlRequest).queueUrl;

        Dto::SQS::ReceiveMessageRequest receiveRequest;
        receiveRequest.region = TEST_REGION;
        receiveRequest.queueUrl = queueUrl;
        receiveRequest.maxMessages = 10;
        receiveRequest.waitTimeSeconds = 0;

        // act
        const Dto::SQS::ReceiveMessageResponse receiveResponse = sqsService.ReceiveMessages(receiveRequest);

        // assert
        BOOST_CHECK_EQUAL(1, receiveResponse.messageList.size());
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
