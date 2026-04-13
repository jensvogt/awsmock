//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_SNS_MAPPER_H
#define AWSMOCK_DTO_SNS_MAPPER_H

// AwsMock includes
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/sns/PublishRequest.h>
#include <awsmock/dto/sns/internal/ListMessageCountersRequest.h>
#include <awsmock/dto/sns/internal/ListMessageCountersResponse.h>
#include <awsmock/dto/sns/internal/ListMessagesRequest.h>
#include <awsmock/dto/sns/internal/ListMessagesResponse.h>
#include <awsmock/dto/sns/internal/ListTopicCountersRequest.h>
#include <awsmock/dto/sns/internal/ListTopicCountersResponse.h>
#include <awsmock/dto/sns/model/Message.h>
#include <awsmock/dto/sns/model/TagCounter.h>
#include <awsmock/dto/sns/model/Topic.h>
#include <awsmock/dto/sqs/internal/ListMessageCountersResponse.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>

namespace AwsMock::Dto::SNS {

    class TopicMapper : public StaticMapper<TopicMapper, Database::Entity::SNS::Topic, Topic> {

      public:

        static Topic toDto(const Database::Entity::SNS::Topic &e) {
            Topic d;
            d.region = e.region;
            d.topicArn = e.topicArn;
            d.topicName = e.topicName;
            d.messages = e.messages;
            d.targetArn = e.targetArn;
            d.size = e.size;
            //d.messageAttributes = MessageAttributeMapper::toDtoMap(e.messageAttributes);
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }

        static Database::Entity::SNS::Topic toEntity(const Topic &d) {
            Database::Entity::SNS::Topic e;
            e.region = d.region;
            e.topicArn = d.topicArn;
            e.topicName = d.topicName;
            e.messages = d.messages;
            e.targetArn = d.targetArn;
            e.size = d.size;
            e.created = d.created;
            e.modified = d.modified;
            return e;
        }
    };

    class TopicCounterMapper : public StaticMapper<TopicCounterMapper, Database::Entity::SNS::Topic, TopicCounter> {

      public:

        static TopicCounter toDto(const Database::Entity::SNS::Topic &e) {
            TopicCounter d;
            d.region = e.region;
            d.topicArn = e.topicArn;
            d.topicName = e.topicName;
            d.messages = e.messages;
            d.messagesSend = e.messagesSend;
            d.messagesResend = e.messagesResend;
            d.size = e.size;
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }

        static Database::Entity::SNS::Topic toEntity(const TopicCounter &d) {
            Database::Entity::SNS::Topic e;
            e.region = d.region;
            e.topicArn = d.topicArn;
            e.topicName = d.topicName;
            e.messages = d.messages;
            e.messagesSend = d.messagesSend;
            e.messagesResend = d.messagesResend;
            e.size = d.size;
            e.created = d.created;
            e.modified = d.modified;
            return e;
        }
    };

    class MessageAttributeMapper : public StaticMapper<MessageAttributeMapper, Database::Entity::SNS::MessageAttribute, MessageAttribute> {

      public:

        static MessageAttribute toDto(const Database::Entity::SNS::MessageAttribute &e) {
            MessageAttribute messageAttribute;
            messageAttribute.dataType = MessageAttributeDataTypeFromString(Database::Entity::SNS::MessageAttributeTypeToString(e.dataType));
            messageAttribute.stringValue = e.stringValue;
            return messageAttribute;
        }

        static Database::Entity::SNS::MessageAttribute toEntity(const MessageAttribute &d) {
            Database::Entity::SNS::MessageAttribute messageAttribute;
            messageAttribute.dataType = Database::Entity::SNS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(d.dataType));
            messageAttribute.stringValue = d.stringValue;
            return messageAttribute;
        }
    };

    class MessageAttributeCounterMapper : public StaticMapper<MessageAttributeCounterMapper, Database::Entity::SNS::MessageAttribute, MessageAttributeCounter> {

      public:

        static MessageAttributeCounter toDto(const Database::Entity::SNS::MessageAttribute &e) {
            MessageAttributeCounter messageAttribute;
            messageAttribute.type = MessageAttributeDataTypeFromString(Database::Entity::SNS::MessageAttributeTypeToString(e.dataType));
            messageAttribute.stringValue = e.stringValue;
            return messageAttribute;
        }

        static Database::Entity::SNS::MessageAttribute toEntity(const MessageAttributeCounter &d) {
            Database::Entity::SNS::MessageAttribute messageAttribute;
            messageAttribute.dataType = Database::Entity::SNS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(d.type));
            messageAttribute.stringValue = d.stringValue;
            return messageAttribute;
        }
    };

    class MessageMapper : public StaticMapper<MessageMapper, Database::Entity::SNS::Message, Message> {

      public:

        static Message toDto(const Database::Entity::SNS::Message &e) {
            Message d;
            d.region = e.region;
            d.topicArn = e.topicArn;
            d.targetArn = e.targetArn;
            d.contentType = e.contentType;
            d.messageId = e.messageId;
            d.message = e.message;
            d.size = e.size;
            d.messageAttributes = MessageAttributeMapper::toDtoMap(e.messageAttributes);
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }

        static Database::Entity::SNS::Message toEntity(const Message &d) {
            Database::Entity::SNS::Message e;
            e.region = d.region;
            e.topicArn = d.topicArn;
            e.targetArn = d.targetArn;
            e.contentType = d.contentType;
            e.messageId = d.messageId;
            e.message = d.message;
            e.size = d.size;
            e.messageAttributes = MessageAttributeMapper::toEntityMap(d.messageAttributes);
            e.created = d.created;
            e.modified = d.modified;
            return e;
        }
    };

    class MessageCounterMapper : public StaticMapper<MessageCounterMapper, Database::Entity::SNS::Message, MessageCounter> {

      public:

        static MessageCounter toDto(const Database::Entity::SNS::Message &e) {
            MessageCounter d;
            d.region = e.region;
            d.topicArn = e.topicArn;
            d.contentType = e.contentType;
            d.messageId = e.messageId;
            d.message = e.message;
            d.size = e.size;
            d.created = e.created;
            d.modified = e.modified;
            for (const auto &[key, value]: e.messageAttributes) {
                MessageAttributeCounter messageAttribute;
                messageAttribute.name = key;
                messageAttribute.type = MessageAttributeDataTypeFromString(Database::Entity::SNS::MessageAttributeTypeToString(value.dataType));
                messageAttribute.stringValue = value.stringValue;
                d.messageAttributes.emplace_back(messageAttribute);
            }
            return d;
        }

        static Database::Entity::SNS::Message toEntity(const Message &d) = delete;
    };

    class PublishRequestMapper : public StaticMapper<PublishRequestMapper, Database::Entity::SNS::Message, PublishRequest> {

      public:

        static Message toDto(const Database::Entity::SNS::Message &e) = delete;

        static Database::Entity::SNS::Message toEntity(const PublishRequest &d) {
            Database::Entity::SNS::Message e;
            e.region = d.region;
            e.topicArn = d.topicArn;
            e.targetArn = d.targetArn;
            e.contentType = d.contentType;
            e.message = d.message;
            e.messageAttributes = MessageAttributeMapper::toEntityMap(d.messageAttributes);
            return e;
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MAPPER_H
