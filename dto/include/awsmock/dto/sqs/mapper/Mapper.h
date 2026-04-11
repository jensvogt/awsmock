//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_SQS_MAPPER_H
#define AWSMOCK_DTO_SQS_MAPPER_H

// AwsMock includes
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/sqs/SendMessageRequest.h>
#include <awsmock/dto/sqs/SendMessageResponse.h>
#include <awsmock/dto/sqs/internal/ListMessageCountersResponse.h>
#include <awsmock/dto/sqs/internal/ListQueueCountersResponse.h>
#include <awsmock/dto/sqs/model/AttributeCounter.h>
#include <awsmock/dto/sqs/model/EventMessageAttribute.h>
#include <awsmock/dto/sqs/model/Message.h>
#include <awsmock/dto/sqs/model/MessageEntry.h>
#include <awsmock/dto/sqs/model/Queue.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/utils/SqsUtils.h>

namespace AwsMock::Dto::SQS {

    class RedrivePolicyMapper : public StaticMapper<RedrivePolicyMapper, Database::Entity::SQS::RedrivePolicy, RedrivePolicy> {
      public:

        static RedrivePolicy toDto(const Database::Entity::SQS::RedrivePolicy &e) {
            RedrivePolicy d;
            d.deadLetterTargetArn = e.deadLetterTargetArn;
            d.maxReceiveCount = e.maxReceiveCount;
            return d;
        }

        static Database::Entity::SQS::RedrivePolicy toEntity(const RedrivePolicy &d) {
            Database::Entity::SQS::RedrivePolicy e;
            e.deadLetterTargetArn = d.deadLetterTargetArn;
            e.maxReceiveCount = d.maxReceiveCount;
            return e;
        }
    };

    class QueueAttributeMapper : public StaticMapper<QueueAttributeMapper, Database::Entity::SQS::QueueAttribute, QueueAttribute> {
      public:

        static QueueAttribute toDto(const Database::Entity::SQS::QueueAttribute &e) {
            QueueAttribute d;
            d.queueArn = e.queueArn;
            d.approximateNumberOfMessages = e.approximateNumberOfMessages;
            d.approximateNumberOfMessagesDelayed = e.approximateNumberOfMessagesDelayed;
            d.approximateNumberOfMessagesNotVisible = e.approximateNumberOfMessagesNotVisible;
            d.policy = e.policy;
            d.delaySeconds = e.delaySeconds;
            d.maxMessageSize = e.maxMessageSize;
            d.messageRetentionPeriod = e.messageRetentionPeriod;
            d.visibilityTimeout = e.visibilityTimeout;
            d.redriveAllowPolicy = e.redriveAllowPolicy;
            d.receiveMessageWaitTime = e.receiveMessageWaitTime;
            d.redrivePolicy = RedrivePolicyMapper::toDto(e.redrivePolicy);
            return d;
        }

        static Database::Entity::SQS::QueueAttribute toEntity(const QueueAttribute &d) {
            Database::Entity::SQS::QueueAttribute e;
            e.queueArn = d.queueArn;
            e.approximateNumberOfMessages = d.approximateNumberOfMessages;
            e.approximateNumberOfMessagesDelayed = d.approximateNumberOfMessagesDelayed;
            e.approximateNumberOfMessagesNotVisible = d.approximateNumberOfMessagesNotVisible;
            e.policy = d.policy;
            e.delaySeconds = d.delaySeconds;
            e.maxMessageSize = d.maxMessageSize;
            e.messageRetentionPeriod = d.messageRetentionPeriod;
            e.visibilityTimeout = d.visibilityTimeout;
            e.redriveAllowPolicy = d.redriveAllowPolicy;
            e.receiveMessageWaitTime = d.receiveMessageWaitTime;
            e.redrivePolicy = RedrivePolicyMapper::toEntity(d.redrivePolicy);
            return e;
        }
    };

    class QueueMapper : public StaticMapper<QueueMapper, Database::Entity::SQS::Queue, Queue> {

      public:

        static Queue toDto(const Database::Entity::SQS::Queue &e) {
            Queue d;
            d.region = e.region;
            d.name = e.name;
            d.arn = e.arn;
            d.url = e.url;
            d.owner = e.owner;
            d.tags = e.tags;
            d.size = e.size;
            d.attributes = QueueAttributeMapper::toDto(e.attributes);
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }

        static Database::Entity::SQS::Queue toEntity(const Queue &d) {
            Database::Entity::SQS::Queue e;
            e.region = d.region;
            e.name = d.name;
            e.url = d.url;
            e.arn = d.arn;
            e.owner = d.owner;
            e.tags = d.tags;
            e.size = d.size;
            e.attributes = QueueAttributeMapper::toEntity(d.attributes);
            e.created = d.created;
            e.modified = d.modified;
            return e;
        }
    };

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a SQS message DTO list to a SQS message entity
         *
         * Some values will be pulled over from the request.
         *
         * @param request request struct
         * @return ListObjectVersionsResponse
         * @see ListObjectVersionsResponse
         */
        static Database::Entity::SQS::Message map(const SendMessageRequest &request);

        /**
         * @brief Maps a SQS queue entity list to a SQS queue DTO
         *
         * @param queueEntity queue entity
         * @return queue DTO
         * @see Queue
         */
        //static Queue map(const Database::Entity::SQS::Queue &queueEntity);

        /**
         * @brief Maps a list of SQS queue entity to a list of SQS queue DTO
         *
         * @param queueEntities list of queue entity
         * @return queue DTO
         * @see Queue
         */
        //static std::vector<Queue> map(const std::vector<Database::Entity::SQS::Queue> &queueEntities);

        /**
         * @brief Maps a list of SQS queue entity to a list of SQS queue URLs
         *
         * @param queueEntities list of queue entity
         * @return queue URLs
         * @see Queue
         */
        static std::vector<std::string> mapUrls(const std::vector<Database::Entity::SQS::Queue> &queueEntities);

        /**
         * @brief Maps a SQS message entity to a SQS send message response DTO
         *
         * Some values will be pulled over from the request.
         *
         * @param request request struct
         * @param messageEntity message entity
         * @return ListObjectVersionsResponse
         * @see ListObjectVersionsResponse
         */
        static SendMessageResponse map(const SendMessageRequest &request, const Database::Entity::SQS::Message &messageEntity);

        /**
         * @brief Maps a SQS message entity to a SQS send message response DTO
         *
         * @param messages message counter list
         * @param total total number of messages
         * @return ListMessageCountersResponse
         * @see ListMessageCountersResponse
         */
        static ListMessageCountersResponse map(const Database::Entity::SQS::MessageList &messages, long total);

        /**
         * @brief Maps a list of SQS queue entity to a ListQueueCounterResponse
         *
         * @param queues queue counter list
         * @param total total number of queues
         * @return ListQueueCountersResponse
         * @see ListQueueCountersResponse
         */
        static ListQueueCountersResponse map(const Database::Entity::SQS::QueueList &queues, long total);

        /**
         * @brief Maps a SQS message entity to a SQS message DTO
         *
         * @param messageEntity message entity
         * @return SQS message DTO
         * @see Message
         */
        static Message map(const Database::Entity::SQS::Message &messageEntity);

        /**
         * @brief Maps a SQS message entity to a SQS message DTO
         *
         * @param messageEntities list of message entity
         * @return list of SQS message DTOs
         * @see Message
         */
        static std::vector<Message> map(const std::vector<Database::Entity::SQS::Message> &messageEntities);

        /**
         * @brief Maps a SQS attribute DTO to a attribute entity
         *
         * @param messageAttributes list of message attributes
         * @return MessageAttributeList
         * @see Database::Entity::SQS::MessageAttributeList
         */
        static Database::Entity::SQS::MessageAttributeList map(const MessageAttributeList &messageAttributes);

        /**
         * @brief Maps a SQS attribute entity to an attribute DTO
         *
         * @param messageAttributes list of message attributes
         * @return MessageAttributeList
         * @see Database::Entity::SQS::MessageAttributeList
         */
        static std::map<std::string, MessageAttribute> map(const std::map<std::string, Database::Entity::SQS::MessageAttribute> &messageAttributes);

        /**
         * @brief Maps a SQS attribute entity to an event message attribute DTO
         *
         * @param messageAttributes list of message attributes
         * @return EventMessageAttributeList
         * @see Database::Entity::SQS::EventMessageAttributeList
         */
        static std::map<std::string, EventMessageAttribute> mapEventMessageAttributes(const std::map<std::string, Database::Entity::SQS::MessageAttribute> &messageAttributes);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MAPPER_H
