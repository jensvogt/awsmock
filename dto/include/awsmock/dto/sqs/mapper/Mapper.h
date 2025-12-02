//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_SQS_MAPPER_H
#define AWSMOCK_DTO_SQS_MAPPER_H

// AwsMock includes
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
        static Queue map(const Database::Entity::SQS::Queue &queueEntity);

        /**
         * @brief Maps a list of SQS queue entity to a list of SQS queue DTO
         *
         * @param queueEntities list of queue entity
         * @return queue DTO
         * @see Queue
         */
        static std::vector<Queue> map(const std::vector<Database::Entity::SQS::Queue> &queueEntities);

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
