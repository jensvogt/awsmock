//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_SQS_SERVICE_H
#define AWSMOCK_SERVICE_SQS_SERVICE_H

// C++ standard includes
#include <string>
#include <thread>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/NotFoundException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/cognito/model/MessageAction.h>
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/sqs/ChangeMessageVisibilityRequest.h>
#include <awsmock/dto/sqs/CreateQueueRequest.h>
#include <awsmock/dto/sqs/CreateQueueResponse.h>
#include <awsmock/dto/sqs/DeleteAttributeRequest.h>
#include <awsmock/dto/sqs/DeleteMessageBatchRequest.h>
#include <awsmock/dto/sqs/DeleteMessageBatchResponse.h>
#include <awsmock/dto/sqs/DeleteMessageRequest.h>
#include <awsmock/dto/sqs/DeleteMessageResponse.h>
#include <awsmock/dto/sqs/DeleteQueueRequest.h>
#include <awsmock/dto/sqs/DeleteQueueResponse.h>
#include <awsmock/dto/sqs/GetQueueAttributesRequest.h>
#include <awsmock/dto/sqs/GetQueueAttributesResponse.h>
#include <awsmock/dto/sqs/GetQueueUrlRequest.h>
#include <awsmock/dto/sqs/GetQueueUrlResponse.h>
#include <awsmock/dto/sqs/ListMessagesRequest.h>
#include <awsmock/dto/sqs/ListMessagesResponse.h>
#include <awsmock/dto/sqs/ListQueueArnsResponse.h>
#include <awsmock/dto/sqs/ListQueueRequest.h>
#include <awsmock/dto/sqs/ListQueueResponse.h>
#include <awsmock/dto/sqs/ListQueueTagsRequest.h>
#include <awsmock/dto/sqs/ListQueueTagsResponse.h>
#include <awsmock/dto/sqs/PurgeQueueRequest.h>
#include <awsmock/dto/sqs/ReceiveMessageRequest.h>
#include <awsmock/dto/sqs/ReceiveMessageResponse.h>
#include <awsmock/dto/sqs/RedriveMessagesRequest.h>
#include <awsmock/dto/sqs/SendMessageBatchRequest.h>
#include <awsmock/dto/sqs/SendMessageBatchResponse.h>
#include <awsmock/dto/sqs/SendMessageRequest.h>
#include <awsmock/dto/sqs/SendMessageResponse.h>
#include <awsmock/dto/sqs/SetQueueAttributesRequest.h>
#include <awsmock/dto/sqs/TagQueueRequest.h>
#include <awsmock/dto/sqs/UntagQueueRequest.h>
#include <awsmock/dto/sqs/internal/GetQueueDetailsRequest.h>
#include <awsmock/dto/sqs/internal/GetQueueDetailsResponse.h>
#include <awsmock/dto/sqs/internal/ListLambdaTriggerCountersRequest.h>
#include <awsmock/dto/sqs/internal/ListLambdaTriggerCountersResponse.h>
#include <awsmock/dto/sqs/internal/ListMessageCountersRequest.h>
#include <awsmock/dto/sqs/internal/ListMessageCountersResponse.h>
#include <awsmock/dto/sqs/internal/ListQueueAttributeCountersRequest.h>
#include <awsmock/dto/sqs/internal/ListQueueAttributeCountersResponse.h>
#include <awsmock/dto/sqs/internal/ListQueueCountersRequest.h>
#include <awsmock/dto/sqs/internal/ListQueueCountersResponse.h>
#include <awsmock/dto/sqs/internal/ListQueueTagCountersRequest.h>
#include <awsmock/dto/sqs/internal/ListQueueTagCountersResponse.h>
#include <awsmock/dto/sqs/internal/ResendMessageRequest.h>
#include <awsmock/dto/sqs/internal/UpdateDqlRequest.h>
#include <awsmock/dto/sqs/internal/UpdateMessageRequest.h>
#include <awsmock/dto/sqs/mapper/Mapper.h>
#include <awsmock/dto/sqs/model/BatchResultErrorEntry.h>
#include <awsmock/dto/sqs/model/DeleteMessageBatchResultEntry.h>
#include <awsmock/dto/sqs/model/EventNotification.h>
#include <awsmock/dto/sqs/model/EventRecord.h>
#include <awsmock/repository/LambdaDatabase.h>
#include <awsmock/repository/SQSDatabase.h>
#include <awsmock/service/lambda/LambdaService.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/monitoring/MetricServiceTimer.h>
#include <awsmock/utils/SqsUtils.h>

namespace AwsMock::Service {
    using std::chrono::system_clock;

    /**
     * @brief SQS service
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSService {
      public:

        /**
         * @brief Constructor
         */
        explicit SQSService() : _sqsDatabase(Database::SQSDatabase::instance()), _lambdaDatabase(Database::LambdaDatabase::instance()) {};

        /**
         * @brief Creates a new queue.
         *
         * <p>In case the queue exists already, return the existing queue.</p>
         *
         * @param request create queue request
         * @return CreateQueueResponse
         */
        [[nodiscard]] Dto::SQS::CreateQueueResponse CreateQueue(const Dto::SQS::CreateQueueRequest &request) const;

        /**
         * @brief Returns a list of all available queues
         *
         * @param request AWS list queue request
         * @return ListQueuesResponse
         * @see ListQueuesRequest
         * @see ListQueuesResponse
         */
        [[nodiscard]] Dto::SQS::ListQueuesResponse ListQueues(const Dto::SQS::ListQueuesRequest &request) const;

        /**
         * @brief Returns a list of all available queues ARNs
         *
         * @return ListQueueArnsResponse
         * @see ListQueueArnsResponse
         */
        [[nodiscard]] Dto::SQS::ListQueueArnsResponse ListQueueArns() const;

        /**
         * @brief Returns a list of all available queues counters
         *
         * @param request list queue counters request
         * @return ListQueueCountersResponse
         * @see ListQueueCountersRequest
         * @see ListQueueCountersResponse
         */
        [[nodiscard]] Dto::SQS::ListQueueCountersResponse ListQueueCounters(const Dto::SQS::ListQueueCountersRequest &request) const;

        /**
         * @brief Returns a list of all available queues tags
         *
         * @param request list queue tagss request
         * @return ListQueueTagsResponse
         * @see ListQueueTagsRequest
         * @see ListQueueTagsResponse
         */
        [[nodiscard]] Dto::SQS::ListQueueTagsResponse ListQueueTags(const Dto::SQS::ListQueueTagsRequest &request) const;

        /**
         * @brief Purge a queue.
         *
         * @param request purge queue request
         * @return total number of deleted queues
         * @throws ServiceException
         */
        [[nodiscard]] long PurgeQueue(const Dto::SQS::PurgeQueueRequest &request) const;

        /**
         * @brief Redrive messages in queue
         *
         * @param request redrive messages request
         * @return total number of redrive messages
         * @throws ServiceException
         */
        [[nodiscard]] long RedriveMessages(const Dto::SQS::RedriveMessagesRequest &request) const;

        /**
         * @brief Return the queue userAttributes
         *
         * @param request get queue sqs request
         * @return GetQueueAttributesResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::GetQueueUrlResponse GetQueueUrl(const Dto::SQS::GetQueueUrlRequest &request) const;

        /**
         * @brief Return the queue userAttributes
         *
         * @param request get queue sqs request
         * @return GetQueueAttributesResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::GetQueueDetailsResponse GetQueueDetails(const Dto::SQS::GetQueueDetailsRequest &request) const;

        /**
         * @brief Return the queue userAttributes
         *
         * @param request get queue sqs request
         * @return GetQueueAttributesResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::GetQueueAttributesResponse GetQueueAttributes(const Dto::SQS::GetQueueAttributesRequest &request) const;

        /**
         * @brief Set queue userAttributes
         *
         * @param request put queue sqs request
         * @throws ServiceException
         */
        void SetQueueAttributes(Dto::SQS::SetQueueAttributesRequest &request) const;

        /**
         * @brief Sets the message visibility timeout.
         *
         * @param request set visibility timeout request
         * @throws ServiceException
         */
        void SetVisibilityTimeout(const Dto::SQS::ChangeMessageVisibilityRequest &request) const;

        /**
         * @brief Sets tags for a queue.
         *
         * @par
         * Existing tags will be updates, and not existing tags will be created.
         *
         * @param request tag resource request DTO
         */
        void TagQueue(const Dto::SQS::TagQueueRequest &request) const;

        /**
         * @brief Removes a tag from a queue.
         *
         * @param request untag resource request DTO
         */
        void UntagQueue(const Dto::SQS::UntagQueueRequest &request) const;

        /**
         * @brief Returns a list of attribute counters for a queue
         *
         * @param request list attribute counters request DTO
         * @return ListAttributeCountersResponse DTO
         */
        [[nodiscard]] Dto::SQS::ListQueueAttributeCountersResponse ListQueueAttributeCounters(const Dto::SQS::ListQueueAttributeCountersRequest &request) const;

        /**
         * @brief Returns a list of lambda trigger counters for a queue
         *
         * @param request list lambda trigger counters request DTO
         * @return ListLambdaTriggerCountersResponse DTO
         */
        [[nodiscard]] Dto::SQS::ListLambdaTriggerCountersResponse ListLambdaTriggerCounters(const Dto::SQS::ListLambdaTriggerCountersRequest &request) const;

        /**
         * @brief Returns a list of tags counters for a queue
         *
         * @param request list tag counters request DTO
         * @return ListTagCountersResponse DTO
         */
        [[nodiscard]] Dto::SQS::ListQueueTagCountersResponse ListTagCounters(const Dto::SQS::ListQueueTagCountersRequest &request) const;

        /**
         * @brief Delete a queue
         *
         * @param request delete request DTO
         * @return SQSQueueResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::DeleteQueueResponse DeleteQueue(const Dto::SQS::DeleteQueueRequest &request) const;

        /**
         * @brief Send a message to the queue
         *
         * @param request send message request
         * @return SendMessageResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::SendMessageResponse SendMessage(const Dto::SQS::SendMessageRequest &request) const;

        /**
         * @brief Creates a new queue
         *
         * @param request create message request
         * @return SendMessageResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::SendMessageBatchResponse SendMessageBatch(const Dto::SQS::SendMessageBatchRequest &request) const;

        /**
         * @brief Receive a list of resources
         *
         * @param request receive message request
         * @return ReceiveMessageResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::ReceiveMessageResponse ReceiveMessages(const Dto::SQS::ReceiveMessageRequest &request) const;

        /**
         * @brief Returns a list SQS messages
         *
         * @param request list messages request
         * @return ListMessagesResponse
         * @throws ServiceException
         * @see ListMessagesResponse
         */
        [[nodiscard]] Dto::SQS::ListMessagesResponse ListMessages(const Dto::SQS::ListMessagesRequest &request) const;

        /**
         * @brief Returns a list SQS messages
         *
         * @param request list messages request
         * @return ListMessagesResponse
         * @throws ServiceException
         * @see ListMessageCountersResponse
         */
        [[nodiscard]] Dto::SQS::ListMessageCountersResponse ListMessageCounters(const Dto::SQS::ListMessageCountersRequest &request) const;

        /**
         * @brief Updates a message
         *
         * @param request update message request DTO
         * @throws ServiceException
         */
        void UpdateMessage(const Dto::SQS::UpdateMessageRequest &request) const;

        /**
         * @brief Resend a message
         *
         * @param request resend message request DTO
         * @throws ServiceException
         * @see ResendMessage
         */
        void ResendMessage(const Dto::SQS::ResendMessageRequest &request) const;

        /**
         * @brief Updates a DQL subscription
         *
         * @param request update DQL subscription request DTO
         * @throws ServiceException
         */
        void UpdateDql(const Dto::SQS::UpdateDqlRequest &request) const;

        /**
        * @brief Deletes a message
        *
        * @param request delete message request DTO
        * @throws ServiceException
        */
        void DeleteMessage(const Dto::SQS::DeleteMessageRequest &request) const;

        /**
         * @brief Deletes a message attribute
         *
         * @param request delete message attribute request DTO
         * @throws ServiceException
         */
        void DeleteMessageAttribute(const Dto::SQS::DeleteAttributeRequest &request) const;

        /**
         * @brief Deletes a message in a batch
         *
         * @param request delete message batch request DTO
         * @return DeleteMessageBatchResponse
         * @throws ServiceException
         */
        [[nodiscard]] Dto::SQS::DeleteMessageBatchResponse DeleteMessageBatch(const Dto::SQS::DeleteMessageBatchRequest &request) const;

      private:

        /**
         * @brief Send a lambda invocation request for a message.
         *
         * @param queueArn queue ARN
         * @param message SQS message.
         */
        void CheckLambdaNotifications(const std::string &queueArn, const Database::Entity::SQS::Message &message) const;

        /**
         * @brief Send a lambda invocation request for a message.
         *
         * @param lambda lambda to invoke.
         * @param message SQS message.
         * @param eventSourceArn event source ARN
         */
        void SendLambdaInvocationRequest(const Database::Entity::Lambda::Lambda &lambda, const Database::Entity::SQS::Message &message, const std::string &eventSourceArn) const;

        /**
         * @brief Checks the attributes for an entry with 'all'. The search is case-insensitive.
         *
         * @param attributes vector of attributes.
         * @param value value to check for.
         */
        static bool CheckAttribute(const std::vector<std::string> &attributes, const std::string &value);

        /**
         * SQS database connection
         */
        Database::SQSDatabase &_sqsDatabase;

        /**
         * Lambda database connection
         */
        Database::LambdaDatabase &_lambdaDatabase;

        /**
         * Lambda service
         */
        LambdaService _lambdaService;
    };
}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_SQS_SERVICE_H
