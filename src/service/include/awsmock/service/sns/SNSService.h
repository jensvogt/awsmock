//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SNS_SERVICE_H
#define AWSMOCK_SERVICE_SNS_SERVICE_H

// C++ standard includes
#include <map>
#include <memory>
#include <string>

// Boost includes
#include <boost/asio/strand.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/PagingUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/dto/dynamodb/mapper/Mapper.h>
#include <awsmock/dto/lambda/model/InvocationType.h>
#include <awsmock/dto/sns/CreateTopicRequest.h>
#include <awsmock/dto/sns/CreateTopicResponse.h>
#include <awsmock/dto/sns/DeleteMessageRequest.h>
#include <awsmock/dto/sns/DeleteTopicResponse.h>
#include <awsmock/dto/sns/GetTopicAttributesRequest.h>
#include <awsmock/dto/sns/GetTopicAttributesResponse.h>
#include <awsmock/dto/sns/ListSubscriptionsByTopicRequest.h>
#include <awsmock/dto/sns/ListSubscriptionsByTopicResponse.h>
#include <awsmock/dto/sns/ListTopicArnsResponse.h>
#include <awsmock/dto/sns/ListTopicsResponse.h>
#include <awsmock/dto/sns/PublishRequest.h>
#include <awsmock/dto/sns/PublishResponse.h>
#include <awsmock/dto/sns/PurgeTopicRequest.h>
#include <awsmock/dto/sns/SqsNotificationRequest.h>
#include <awsmock/dto/sns/SubscribeRequest.h>
#include <awsmock/dto/sns/SubscribeResponse.h>
#include <awsmock/dto/sns/TagResourceRequest.h>
#include <awsmock/dto/sns/TagResourceResponse.h>
#include <awsmock/dto/sns/UnsubscribeRequest.h>
#include <awsmock/dto/sns/UnsubscribeResponse.h>
#include <awsmock/dto/sns/UntagResourceRequest.h>
#include <awsmock/dto/sns/UntagResourceResponse.h>
#include <awsmock/dto/sns/UpdateSubscriptionRequest.h>
#include <awsmock/dto/sns/UpdateSubscriptionResponse.h>
#include <awsmock/dto/sns/internal/GetMessageCountersRequest.h>
#include <awsmock/dto/sns/internal/GetMessageCountersResponse.h>
#include <awsmock/dto/sns/internal/GetEventSourceRequest.h>
#include <awsmock/dto/sns/internal/GetEventSourceResponse.h>
#include <awsmock/dto/sns/internal/GetTopicDetailsRequest.h>
#include <awsmock/dto/sns/internal/GetTopicDetailsResponse.h>
#include <awsmock/dto/sns/internal/ListAttributeCountersRequest.h>
#include <awsmock/dto/sns/internal/ListAttributeCountersResponse.h>
#include <awsmock/dto/sns/internal/ListMessageCountersRequest.h>
#include <awsmock/dto/sns/internal/ListMessageCountersResponse.h>
#include <awsmock/dto/sns/internal/ListMessagesRequest.h>
#include <awsmock/dto/sns/internal/ListMessagesResponse.h>
#include <awsmock/dto/sns/internal/ListSubscriptionCountersRequest.h>
#include <awsmock/dto/sns/internal/ListSubscriptionCountersResponse.h>
#include <awsmock/dto/sns/internal/ListTagCountersRequest.h>
#include <awsmock/dto/sns/internal/ListTagCountersResponse.h>
#include <awsmock/dto/sns/internal/ListTopicCountersRequest.h>
#include <awsmock/dto/sns/internal/ListTopicCountersResponse.h>
#include <awsmock/dto/sns/mapper/Mapper.h>
#include <awsmock/dto/sns/model/EventNotification.h>
#include <awsmock/dto/sns/model/EventRecord.h>
#include <awsmock/dto/sqs/SendMessageRequest.h>
#include <awsmock/dto/sqs/SendMessageResponse.h>
#include <awsmock/repository/SNSDatabase.h>
#include <awsmock/service/lambda/LambdaService.h>
#include <awsmock/service/sqs/SQSService.h>
#include <awsmock/utils/SqsUtils.h>
#include <awsmock/dto/sns/model/DeleteTopicRequest.h>

#define SQS_PROTOCOL "sqs"
#define HTTP_PROTOCOL "http"
#define HTTPS_PROTOCOL "https"
#define LAMBDA_ENDPOINT "lambda"
#define DEFAULT_SQS_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    /**
     * @brief SNS service thread
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSService /*: public std::enable_shared_from_this<SNSService>*/ {

    public:

        /**
         * @brief Constructor
         */
        explicit SNSService(boost::asio::io_context &ioc) : _snsDatabase(Database::SNSDatabase::instance()), _sqsDatabase(Database::SQSDatabase::instance()), _lambdaDatabase(Database::LambdaDatabase::instance()), _sqsService(ioc), _lambdaService(ioc), _ioc(ioc) {}

        /**
         * @brief Creates a new topic
         *
         * <p>In case the topic exists already, return the existing topic.</p>
         *
         * @param request create topic request
         * @return CreateTopicResponse
         */
        [[nodiscard]] Dto::SNS::CreateTopicResponse CreateTopic(const Dto::SNS::CreateTopicRequest &request) const;

        /**
         * @brief Returns a list of all available topics
         *
         * @param region AWS region
         * @return ListTopicsResponse
         * @see ListTopicsResponse
         */
        [[nodiscard]] Dto::SNS::ListTopicsResponse ListTopics(const std::string &region) const;

        /**
         * @brief Returns a list of all available topic ARNs
         *
         * @param region AWS region
         * @return ListTopicArnsResponse
         * @see ListTopicArnsResponse
         */
        [[nodiscard]] Dto::SNS::ListTopicArnsResponse ListTopicArns(const std::string &region) const;

        /**
         * @brief Returns a list of all topic counters
         *
         * @param request List topic counters request
         * @return ListTopicCountersResponse
         * @see ListTopicCountersResponse
         * @see ListTopicCountersResponse
         */
        [[nodiscard]] Dto::SNS::ListTopicCountersResponse ListTopicCounters(const Dto::SNS::ListTopicCountersRequest &request) const;

        /**
         * @brief Publish a message to an SNS topic
         *
         * @param request AWS region
         * @return PublishResponse
         */
        [[nodiscard]] Dto::SNS::PublishResponse Publish(const Dto::SNS::PublishRequest &request) const;

        /**
         * @brief Subscribe to a topic
         *
         * @param request subscribe request DTO
         * @return SubscribeResponse DTO
         */
        [[nodiscard]] Dto::SNS::SubscribeResponse Subscribe(const Dto::SNS::SubscribeRequest &request) const;

        /**
         * @brief Unsubscribe from a topic
         *
         * @param request unsubscribe request DTO
         * @return UnsubscribeResponse DTO
         * @see UnsubscribeRequest
         * @see UnsubscribeResponse
         */
        [[nodiscard]] Dto::SNS::UnsubscribeResponse Unsubscribe(const Dto::SNS::UnsubscribeRequest &request) const;

        /**
         * @brief Update a topic subscription
         *
         * @param request update subscription request DTO
         * @return UpdateSubscriptionResponse DTO
         * @see UpdateSubscriptionRequest
         * @see UpdateSubscriptionResponse
         */
        [[nodiscard]] Dto::SNS::UpdateSubscriptionResponse UpdateSubscription(const Dto::SNS::UpdateSubscriptionRequest &request) const;

        /**
         * @brief Sets tags for a topic
         *
         * @param request tag resource request DTO
         * @return TagResourceResponse DTO
         * @see TagResourceRequest DTO
         * @see TagResourceResponse DTO
         */
        [[nodiscard]] Dto::SNS::TagResourceResponse TagResource(const Dto::SNS::TagResourceRequest &request) const;

        /**
         * @brief Removes tags from a topic
         *
         * @param request untag resource request DTO
         * @return UntagResourceResponse DTO
         * @see UntagRequestResponse DTO
         * @see UntagResourceResponse DTO
         */
        [[nodiscard]] Dto::SNS::UntagResourceResponse UntagResource(const Dto::SNS::UntagResourceRequest &request) const;

        /**
         * @brief Returns the topic attributes
         *
         * @param request get topic attributes request DTO
         * @return GetTopicAttributesResponse DTO
         * @see GetTopicAttributesRequest
         * @see GetTopicAttributesResponse
         */
        [[nodiscard]] Dto::SNS::GetTopicAttributesResponse GetTopicAttributes(const Dto::SNS::GetTopicAttributesRequest &request) const;

        /**
         * @brief Returns the topic details.
         *
         * @param request get topic details request DTO
         * @return GetTopicDetailsResponse DTO
         * @see GetTopicDetailsRequest
         * @see GetTopicDetailsResponse
         */
        [[nodiscard]] Dto::SNS::GetTopicDetailsResponse GetTopicDetails(const Dto::SNS::GetTopicDetailsRequest &request) const;

        /**
         * @brief Returns a list of subscriptions for a topic
         *
         * @param request list subscriptions request DTO
         * @return ListSubscriptionByTopicResponse DTO
         */
        [[nodiscard]] Dto::SNS::ListSubscriptionsByTopicResponse ListSubscriptionsByTopic(const Dto::SNS::ListSubscriptionsByTopicRequest &request) const;

        /**
         * @brief Returns a list of subscription counters for a topic
         *
         * @param request list subscription counters request DTO
         * @return ListSubscriptionCountersResponse DTO
         */
        [[nodiscard]] Dto::SNS::ListSubscriptionCountersResponse ListSubscriptionCounters(const Dto::SNS::ListSubscriptionCountersRequest &request) const;

        /**
         * @brief Returns a list of tags counters for a topic
         *
         * @param request list tag counters request DTO
         * @return ListTagCountersResponse DTO
         */
        [[nodiscard]] Dto::SNS::ListTagCountersResponse ListTagCounters(const Dto::SNS::ListTagCountersRequest &request) const;

        /**
         * @brief Returns a list of attribute counters for a topic
         *
         * @param request list attribute counters request DTO
         * @return ListAttributeCountersResponse DTO
         */
        [[nodiscard]] Dto::SNS::ListAttributeCountersResponse ListAttributeCounters(const Dto::SNS::ListAttributeCountersRequest &request) const;

        /**
         * @brief Purge a topic
         *
         * @param request purge topic request
         * @return total number of deleted messages
         * @throws ServiceException
         */
        [[nodiscard]] long PurgeTopic(const Dto::SNS::PurgeTopicRequest &request) const;

        /**
         * @brief Purge all topics
         *
         * @return total number of deleted messages
         * @throws ServiceException
         */
        [[nodiscard]] long PurgeAllTopics() const;

        /**
         * @brief Returns an event source as a lambda configuration
         *
         * @param request get event source request
         * @return Dto::S3::GetEventSourceResponse
         */
        [[nodiscard]] Dto::SNS::GetEventSourceResponse GetEventSource(const Dto::SNS::GetEventSourceRequest &request) const;

        /**
         * @brief Delete a topic
         *
         * @param request delete request
         * @return DeleteTopicResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SNS::DeleteTopicResponse DeleteTopic(const Dto::SNS::DeleteTopicRequest &request) const;

        /**
         * @brief List messages
         *
         * @param request list messages request
         * @return ListmessagesResponse
         * @see ListmessagesResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SNS::ListMessagesResponse ListMessages(const Dto::SNS::ListMessagesRequest &request) const;

        /**
         * @brief List message counters
         *
         * @param request list message counters request
         * @return ListmessageCountersResponse
         * @see ListmessageCountersResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SNS::ListMessageCountersResponse ListMessageCounters(const Dto::SNS::ListMessageCountersRequest &request) const;

        /**
         * @brief Get message counters
         *
         * @param request get message counters request
         * @return GetMessageCountersResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SNS::GetMessageCountersResponse GetMessageCounters(const Dto::SNS::GetMessageCountersRequest &request) const;

        /**
         * @brief Deletes a message
         *
         * @par
         * This is not an official AWS SDK service, but exists in AwsMock.
         *
         * @param request delete message request
         * @see DeleteMessageRequest
         * @throws ServiceException
         */
        void DeleteMessage(const Dto::SNS::DeleteMessageRequest &request) const;

    private:

        /**
         * @brief Checks the subscriptions.
         *
         * @par
         * If a SQS topic subscription is found, send the message to the SQS topic.
         *
         * @param request SNS publish request
         * @param topic SNS topic entity
         * @param message SNS message entity
         */
        void CheckSubscriptions(const Dto::SNS::PublishRequest &request, const Database::Entity::SNS::Topic &topic, const Database::Entity::SNS::Message &message) const;

        /**
         * @brief Send an SNS message to an SQS topic
         *
         * @param subscription SNS subscription
         * @param request SNS publish request
         */
        void SendSQSMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request) const;

        /**
         * @brief Adjust topic counters
         *
         * @param topic SNS topic entity
         */
        void AdjustTopicCounters(Database::Entity::SNS::Topic &topic) const;

        /**
         * @brief Send an SNS message to an HTTP endpoint.
         *
         * @param subscription SNS subscription
         * @param request SNS publish request
        */
        static void SendHttpMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request);

        /**
         * @brief Send a lambda invocation request for a message.
         *
         * @param subscription SNS subscription
         * @param request SNS publish request.
         * @param message SNS message.
         */
        void SendLambdaMessage(const Database::Entity::SNS::Subscription &subscription, const Dto::SNS::PublishRequest &request, const Database::Entity::SNS::Message &message) const;

        /**
         * @brief Send an SNS message to a lambda function endpoint.
         *
         * @param lambda lambda function to invoke
         * @param message SNS message to publish
         * @param eventSourceArn event source ARN
        */
        void SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const Database::Entity::SNS::Message &message, const std::string &eventSourceArn) const;

        /**
         * SNS database connection
         */
        Database::SNSDatabase &_snsDatabase;

        /**
         * SQS database connection
         */
        Database::SQSDatabase &_sqsDatabase;

        /**
         * Lambda database connection
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * SQS module
         */
        SQSService _sqsService;

        /**
         * Lambda service
         */
        LambdaService _lambdaService;

        /**
         * Boost asio IO context
         */
        boost::asio::io_context &_ioc;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SNS_SERVICE_H
