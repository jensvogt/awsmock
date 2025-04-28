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
            response.messageList.emplace_back(entity);
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
}// namespace AwsMock::Dto::SNS