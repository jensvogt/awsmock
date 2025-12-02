//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/sns/mapper/Mapper.h>

namespace AwsMock::Dto::SNS {

    ListMessagesResponse Mapper::map(const ListMessagesRequest &request, const std::vector<Database::Entity::SNS::Message> &messageEntities) {

        ListMessagesResponse response{};
        for (const auto &entity: messageEntities) {
            Message message;
            message.region = entity.region;
            message.topicArn = entity.topicArn;
            message.messageId = entity.messageId;
            message.created = entity.created;
            message.modified = entity.modified;
            response.messages.emplace_back(message);
        }
        return response;
    }

    ListTopicCountersResponse Mapper::map(const ListTopicCountersRequest &request, const std::vector<Database::Entity::SNS::Topic> &topicEntities) {

        ListTopicCountersResponse response;
        response.requestId = request.requestId;
        response.region = request.region;
        response.user = request.user;
        for (const auto &entity: topicEntities) {
            TopicCounter counter;
            counter.region = entity.region;
            counter.user = request.user;
            counter.requestId = request.requestId;
            counter.topicName = entity.topicName;
            counter.topicArn = entity.topicArn;
            counter.messages = entity.messages;
            counter.messagesSend = entity.messagesSend;
            counter.messagesResend = entity.messagesResend;
            counter.size = entity.size;
            counter.created = entity.created;
            counter.modified = entity.modified;
            response.topicCounters.emplace_back(counter);
        }
        return response;
    }

    ListMessageCountersResponse Mapper::map(const ListMessageCountersRequest &request, const std::vector<Database::Entity::SNS::Message> &messageEntities) {

        ListMessageCountersResponse response;
        response.requestId = request.requestId;
        response.region = request.region;
        response.user = request.user;
        for (const auto &entity: messageEntities) {
            MessageCounter message;
            message.region = entity.region;
            message.topicArn = entity.topicArn;
            message.messageId = entity.messageId;
            message.message = entity.message;
            message.size = entity.size;
            message.contentType = entity.contentType;
            message.messageStatus = MessageStatusFromString(Database::Entity::SNS::MessageStatusToString(entity.status));
            message.lastSend = entity.lastSend;
            message.created = entity.created;
            message.modified = entity.modified;
            for (const auto &[fst, snd]: entity.messageAttributes) {
                MessageAttributeCounter attribute;
                attribute.name = fst;
                attribute.stringValue = snd.stringValue;
                attribute.type = MessageAttributeDataTypeFromString(Database::Entity::SNS::MessageAttributeTypeToString(snd.dataType));
                message.messageAttributes.emplace_back(attribute);
            }
            response.messages.emplace_back(message);
        }
        return response;
    }

    Message Mapper::map(const Database::Entity::SNS::Message &messageEntity) {
        Message messageDto;
        messageDto.region = messageEntity.region;
        messageDto.messageId = messageEntity.messageId;
        messageDto.topicArn = messageEntity.topicArn;
        messageDto.message = messageEntity.message;
        messageDto.contentType = messageEntity.contentType;
        messageDto.messageAttributes = map(messageEntity.messageAttributes);
        messageDto.created = messageEntity.created;
        messageDto.modified = messageEntity.modified;
        return messageDto;
    }

    Database::Entity::SNS::Message Mapper::map(const Message &messageDto) {
        Database::Entity::SNS::Message messageEntity;
        messageEntity.region = messageDto.region;
        messageEntity.messageId = messageDto.messageId;
        messageEntity.topicArn = messageDto.topicArn;
        messageEntity.message = messageDto.message;
        messageEntity.contentType = messageDto.contentType;
        messageEntity.messageAttributes = map(messageDto.messageAttributes);
        messageEntity.created = messageDto.created;
        messageEntity.modified = messageDto.modified;
        return messageEntity;
    }

    Database::Entity::SNS::MessageAttribute Mapper::map(const MessageAttribute &messageAttribute) {
        Database::Entity::SNS::MessageAttribute messageAttributeEntity;
        messageAttributeEntity.dataType = Database::Entity::SNS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(messageAttribute.dataType));
        messageAttributeEntity.stringValue = messageAttribute.stringValue;
        return messageAttributeEntity;
    }

    std::map<std::string, Database::Entity::SNS::MessageAttribute> Mapper::map(const std::map<std::string, MessageAttribute> &messageAttributes) {
        std::map<std::string, Database::Entity::SNS::MessageAttribute> messageAttributesEntities;
        for (const auto &[fst, snd]: messageAttributes) {
            const Database::Entity::SNS::MessageAttribute entity = map(snd);
            messageAttributesEntities[fst] = entity;
        }
        return messageAttributesEntities;
    }

    MessageAttribute Mapper::map(const Database::Entity::SNS::MessageAttribute &messageAttributeEntity) {
        MessageAttribute messageAttribute;
        messageAttribute.dataType = MessageAttributeDataTypeFromString(Database::Entity::SNS::MessageAttributeTypeToString(messageAttributeEntity.dataType));
        messageAttribute.stringValue = messageAttributeEntity.stringValue;
        return messageAttribute;
    }

    std::map<std::string, MessageAttribute> Mapper::map(const std::map<std::string, Database::Entity::SNS::MessageAttribute> &messageAttributesEntities) {
        std::map<std::string, MessageAttribute> messageAttributes;
        for (const auto &[fst, snd]: messageAttributesEntities) {
            messageAttributes[fst] = map(snd);
        }
        return messageAttributes;
    }
}// namespace AwsMock::Dto::SNS