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
            counter.availableMessages = entity.topicAttribute.availableMessages;
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
            message.messageSatus = MessageStatusFromString(Database::Entity::SNS::MessageStatusToString(entity.status));
            message.created = entity.created;
            message.modified = entity.modified;
            for (const auto &[attributeName, attributeValue, attributeType]: entity.messageAttributes) {
                MessageAttributeCounter attribute;
                attribute.name = attributeName;
                attribute.stringValue = attributeValue;
                attribute.type = MessageAttributeDataTypeFromString(MessageAttributeTypeToString(attributeType));
                message.messageAttributes.emplace_back(attribute);
            }
            response.messages.emplace_back(message);
        }
        return response;
    }

    Database::Entity::SNS::MessageAttribute Mapper::map(const MessageAttribute &messageAttribute) {
        Database::Entity::SNS::MessageAttribute messageAttributeEntity;
        messageAttributeEntity.attributeType = Database::Entity::SNS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(messageAttribute.type));
        messageAttributeEntity.attributeName = messageAttribute.name;
        if (messageAttributeEntity.attributeType == Database::Entity::SNS::MessageAttributeType::STRING) {
            messageAttributeEntity.attributeValue = messageAttribute.stringValue;
        } else if (messageAttributeEntity.attributeType == Database::Entity::SNS::MessageAttributeType::NUMBER) {
            messageAttributeEntity.attributeValue = std::to_string(messageAttribute.numberValue);
        }
        return messageAttributeEntity;
    }

    Database::Entity::SNS::MessageAttributeList Mapper::map(const std::map<std::string, MessageAttribute> &messageAttributes) {
        std::vector<Database::Entity::SNS::MessageAttribute> messageAttributesEntities;
        for (const auto &snd: messageAttributes | std::views::values) {
            messageAttributesEntities.emplace_back(map(snd));
        }
        return messageAttributesEntities;
    }

    MessageAttribute Mapper::map(const Database::Entity::SNS::MessageAttribute &messageAttributeEntity) {
        MessageAttribute messageAttribute;
        messageAttribute.type = MessageAttributeDataTypeFromString(Database::Entity::SNS::MessageAttributeTypeToString(messageAttributeEntity.attributeType));
        messageAttribute.name = messageAttributeEntity.attributeName;
        if (messageAttribute.type == STRING) {
            messageAttribute.stringValue = messageAttributeEntity.attributeValue;
        } else if (messageAttribute.type == NUMBER) {
            messageAttribute.numberValue = std::stol(messageAttributeEntity.attributeValue);
        }
        return messageAttribute;
    }

    std::map<std::string, MessageAttribute> Mapper::map(const Database::Entity::SNS::MessageAttributeList &messageAttributesEntities) {
        std::map<std::string, MessageAttribute> messageAttributes;
        for (const auto &messageAttributeEntity: messageAttributesEntities) {
            messageAttributes[messageAttributeEntity.attributeName] = map(messageAttributeEntity);
        }
        return messageAttributes;
    }
}// namespace AwsMock::Dto::SNS