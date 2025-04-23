//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/sqs/mapper/Mapper.h>

namespace AwsMock::Dto::SQS {

    //    std::vector<std::string> Mapper::excludedAttributeNames = {"contentType", "id", "timestamp"};

    Database::Entity::SQS::Message Mapper::map(const SendMessageRequest &request) {

        Database::Entity::SQS::Message messageEntity;
        messageEntity.body = request.body,
        messageEntity.size = static_cast<long>(request.body.length()),
        messageEntity.contentType = request.contentType;

        for (const auto &[fst, snd]: request.messageAttributes) {
            Database::Entity::SQS::MessageAttribute attribute = {.attributeName = fst, .attributeValue = snd.stringValue, .attributeType = Database::Entity::SQS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(snd.type))};
            messageEntity.messageAttributes.emplace_back(attribute);
        }
        return messageEntity;
    }

    SendMessageResponse Mapper::map(const SendMessageRequest &request, const Database::Entity::SQS::Message &messageEntity) {

        SendMessageResponse response;
        response.queueUrl = request.queueUrl,
        response.messageId = messageEntity.messageId,
        response.receiptHandle = messageEntity.receiptHandle,
        response.md5Body = messageEntity.md5Body,
        response.md5MessageAttributes = messageEntity.md5MessageAttributes,
        response.md5SystemAttributes = messageEntity.md5SystemAttributes,
        response.requestId = request.requestId;
        return response;
    }

    ListMessageCountersResponse Mapper::map(const Database::Entity::SQS::MessageList &messages, const long total) {

        ListMessageCountersResponse listMessageCountersResponse;
        listMessageCountersResponse.total = total;

        for (const auto &message: messages) {
            MessageCounter messageCounter;
            messageCounter.messageId = message.messageId;
            messageCounter.id = message.oid;
            messageCounter.body = message.body;
            messageCounter.receiptHandle = message.receiptHandle;
            messageCounter.md5Sum = message.md5Body;
            messageCounter.retries = message.retries;
            messageCounter.size = message.size;
            messageCounter.attributes = message.attributes;
            messageCounter.messageAttributes = map(message.messageAttributes);
            messageCounter.created = message.created;
            messageCounter.modified = message.modified;
            listMessageCountersResponse.messages.emplace_back(messageCounter);
        }
        return listMessageCountersResponse;
    }

    Database::Entity::SQS::MessageAttributeList Mapper::map(const MessageAttributeList &messageAttributes) {
        Database::Entity::SQS::MessageAttributeList messageAttributeList{};
        for (const auto &[fst, snd]: messageAttributes) {
            Database::Entity::SQS::MessageAttribute messageAttribute;
            messageAttribute.attributeName = fst;
            messageAttribute.attributeValue = snd.stringValue;
            messageAttribute.attributeType = Database::Entity::SQS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(snd.type));
            messageAttributeList.emplace_back(messageAttribute);
        }
        return messageAttributeList;
    }

    MessageAttributeList Mapper::map(const Database::Entity::SQS::MessageAttributeList &messageAttributes) {
        MessageAttributeList messageAttributeList{};
        if (!messageAttributes.empty()) {
            for (const auto &[attributeName, attributeValue, attributeType]: messageAttributes) {
                MessageAttribute messageAttribute;
                messageAttribute.name = attributeName;
                messageAttribute.type = MessageAttributeDataTypeFromString(MessageAttributeTypeToString(attributeType));
                messageAttribute.stringValue = attributeValue;
                messageAttributeList[attributeName] = messageAttribute;
            }
        }
        return messageAttributeList;
    }

}// namespace AwsMock::Dto::SQS