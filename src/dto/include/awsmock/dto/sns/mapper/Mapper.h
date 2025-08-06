//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_SNS_MAPPER_H
#define AWSMOCK_DTO_SNS_MAPPER_H

// AwsMock includes
#include <awsmock/dto/sns/internal/ListMessageCountersRequest.h>
#include <awsmock/dto/sns/internal/ListMessageCountersResponse.h>
#include <awsmock/dto/sns/internal/ListMessagesRequest.h>
#include <awsmock/dto/sns/internal/ListMessagesResponse.h>
#include <awsmock/dto/sns/internal/ListTopicCountersRequest.h>
#include <awsmock/dto/sns/internal/ListTopicCountersResponse.h>
#include <awsmock/dto/sns/model/Message.h>
#include <awsmock/dto/sqs/internal/ListMessageCountersResponse.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>


namespace AwsMock::Dto::SNS {

    /**
     * @brief Maps an entity to the corresponding DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps an SNS message entity list to a list messages response
         *
         * Some values will be pulled over from the request.
         *
         * @param request request struct
         * @param messageEntities SNS message entity list
         * @return ListMessagesResponse
         * @see ListMessagesResponse
         * @see ListMessagesRequest
         */
        static ListMessagesResponse map(const ListMessagesRequest &request, const std::vector<Database::Entity::SNS::Message> &messageEntities);

        /**
         * @brief Maps an SNS message entity list to a list messages response
         *
         * Some values will be pulled over from the request.
         *
         * @param request original request
         * @param topicEntities SNS topic entity list
         * @return ListMessagesResponse
         * @see ListMessagesResponse
         * @see ListMessagesRequest
         */
        static ListTopicCountersResponse map(const ListTopicCountersRequest &request, const std::vector<Database::Entity::SNS::Topic> &topicEntities);

        /**
         * @brief Maps an SNS message entity list to a list messages response
         *
         * Some values will be pulled over from the request.
         *
         * @param request original request
         * @param messageEntities SNS message entity list
         * @return ListMessagesResponse
         * @see ListMessagesResponse
         * @see ListMessagesRequest
         */
        static ListMessageCountersResponse map(const ListMessageCountersRequest &request, const std::vector<Database::Entity::SNS::Message> &messageEntities);

        /**
         * @brief Maps an SNS message entity list to a message DTO
         *
         * Some values will be pulled over from the request.
         *
         * @param messageEntity message entity
         * @return Message DTO
         * @see Database::Entity::SNS::Message
         * @see Message
         */
        static Message map(const Database::Entity::SNS::Message &messageEntity);

        /**
         * @brief Maps an SNS message DTO list to message entity
         *
         * @param messageDto message DTO
         * @return Message entity
         * @see Database::Entity::SNS::Message
         * @see Message
         */
        static Database::Entity::SNS::Message map(const Message &messageDto);

        /**
         * @brief Maps a single SNS message attribute DTO to a message attribute entity
         *
         * @param messageAttribute SNS message attribute DTO list
         * @return MessageAttribute entity
         * @see SQS::MessageAttributeList
         * @see Database::Entity::SQS::MessageAttributeList
         */
        static Database::Entity::SNS::MessageAttribute map(const MessageAttribute &messageAttribute);

        /**
         * @brief Maps an SNS message attribute DTO to a message attribute entity
         *
         * @param messageAttributes SNS message attribute DTO list
         * @return MessageAttributeList
         * @see SQS::MessageAttributeList
         * @see Database::Entity::SQS::MessageAttributeList
         */
        static std::map<std::string, Database::Entity::SNS::MessageAttribute> map(const std::map<std::string, MessageAttribute> &messageAttributes);

        /**
         * @brief Maps a single SNS message attribute entity to a message attribute DTO
         *
         * @param messageAttributeEntity SNS message attribute entities list
         * @return MessageAttribute DTo
         * @see SQS::MessageAttribute
         * @see Database::Entity::SQS::MessageAttribute
         */
        static MessageAttribute map(const Database::Entity::SNS::MessageAttribute &messageAttributeEntity);

        /**
         * @brief Maps an SNS message attribute DTO to a message attribute entity
         *
         * @param messageAttributesEntities SNS message attribute entities list
         * @return MessageAttributeList DTos
         * @see SQS::MessageAttributeList
         * @see Database::Entity::SQS::MessageAttributeList
         */
        static std::map<std::string, MessageAttribute> map(const std::map<std::string, Database::Entity::SNS::MessageAttribute> &messageAttributesEntities);
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MAPPER_H
