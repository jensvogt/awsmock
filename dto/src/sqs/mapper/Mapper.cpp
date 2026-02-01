//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/sqs/mapper/Mapper.h>

namespace AwsMock::Dto::SQS {
    struct EventMessageAttribute;

    Queue Mapper::map(const Database::Entity::SQS::Queue &queueEntity) {

        Queue queueDto;
        queueDto.name = queueEntity.name;
        queueDto.owner = queueEntity.owner;
        queueDto.queueArn = queueEntity.queueArn;
        queueDto.queueUrl = queueEntity.queueUrl;
        queueDto.size = queueEntity.size;
        queueDto.mainQueue = queueEntity.mainQueue;
        queueDto.tags = queueEntity.tags;
        queueDto.isDlq = queueEntity.isDlq;
        queueDto.created = queueEntity.created;
        queueDto.modified = queueEntity.modified;

        queueDto.attributes.attributeName = "DelaySeconds";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.delaySeconds);
        queueDto.attributes.attributeName = "MaxMessageSize";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.maxMessageSize);
        queueDto.attributes.attributeName = "MessageRetentionPeriod";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.messageRetentionPeriod);
        queueDto.attributes.attributeName = "VisibilityTimeout";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.visibilityTimeout);
        queueDto.attributes.attributeName = "ReceiveMessageWaitTimeSeconds";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.receiveMessageWaitTime);
        queueDto.attributes.attributeName = "CreatedTimestamp";
        queueDto.attributes.attributeValue = std::to_string(Core::DateTimeUtils::UnixTimestamp(queueEntity.created));
        queueDto.attributes.attributeName = "LastModifiedTimestamp";
        queueDto.attributes.attributeValue = std::to_string(Core::DateTimeUtils::UnixTimestamp(queueEntity.modified));
        queueDto.attributes.attributeName = "ApproximateNumberOfMessages";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.approximateNumberOfMessages);
        queueDto.attributes.attributeName = "ApproximateNumberOfMessagesDelayed";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.approximateNumberOfMessagesDelayed);
        queueDto.attributes.attributeName = "ApproximateNumberOfMessagesNotVisible";
        queueDto.attributes.attributeValue = std::to_string(queueEntity.attributes.approximateNumberOfMessagesNotVisible);
        queueDto.attributes.attributeName = "Policy";
        queueDto.attributes.attributeValue = queueEntity.attributes.policy;
        queueDto.attributes.attributeName = "RedriveAllowPolicy";
        queueDto.attributes.attributeValue = queueEntity.attributes.redriveAllowPolicy;
        queueDto.attributes.attributeName = "QueueArn";
        queueDto.attributes.attributeValue = queueEntity.attributes.queueArn;
        for (const auto &[fst, snd]: queueEntity.defaultMessageAttributes) {
            MessageAttribute attribute;
            attribute.stringValue = snd.stringValue;
            attribute.stringListValues = snd.stringListValues;
            attribute.dataType = MessageAttributeDataTypeFromString(Database::Entity::SQS::MessageAttributeTypeToString(snd.dataType));
            queueDto.defaultMessageAttributes[fst] = attribute;
        }
        return queueDto;
    }

    std::vector<Queue> Mapper::map(const std::vector<Database::Entity::SQS::Queue> &queueEntities) {
        std::vector<Queue> queueDtos;
        for (const auto &queueEntity: queueEntities) {
            queueDtos.emplace_back(map(queueEntity));
        }
        return queueDtos;
    }

    std::vector<std::string> Mapper::mapUrls(const std::vector<Database::Entity::SQS::Queue> &queueEntities) {
        std::vector<std::string> queueUrls;
        for (const auto &queueEntity: queueEntities) {
            queueUrls.emplace_back(queueEntity.queueUrl);
        }
        return queueUrls;
    }

    Database::Entity::SQS::Message Mapper::map(const SendMessageRequest &request) {

        Database::Entity::SQS::Message messageEntity;
        messageEntity.body = request.body;

        for (const auto &[fst, snd]: request.messageAttributes) {
            Database::Entity::SQS::MessageAttribute attribute;
            attribute.stringValue = snd.stringValue;
            attribute.stringListValues = snd.stringListValues;
            attribute.dataType = Database::Entity::SQS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(snd.dataType));
            messageEntity.messageAttributes[fst] = attribute;
        }
        for (const auto &[fst, snd]: request.attributes) {
            messageEntity.attributes[fst] = snd;
        }
        return messageEntity;
    }

    SendMessageResponse Mapper::map(const SendMessageRequest &request, const Database::Entity::SQS::Message &messageEntity) {

        SendMessageResponse response;
        response.messageId = messageEntity.messageId,
        response.md5Body = messageEntity.md5Body,
        response.md5MessageAttributes = messageEntity.md5MessageAttributes,
        response.md5MessageSystemAttributes = messageEntity.md5MessageSystemAttributes,
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
            messageCounter.queueName = message.queueName;
            messageCounter.queueArn = message.queueArn;
            messageCounter.queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(message.queueArn);
            messageCounter.body = message.body;
            messageCounter.contentType = message.contentType;
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

    ListQueueCountersResponse Mapper::map(const Database::Entity::SQS::QueueList &queues, const long total) {

        ListQueueCountersResponse listQueueCountersResponse;
        listQueueCountersResponse.total = total;

        for (const auto &queue: queues) {
            QueueCounter counter;
            counter.queueName = queue.name;
            counter.queueArn = queue.queueArn;
            counter.queueUrl = queue.queueUrl;
            counter.available = queue.attributes.approximateNumberOfMessages;
            counter.invisible = queue.attributes.approximateNumberOfMessagesNotVisible;
            counter.delayed = queue.attributes.approximateNumberOfMessagesDelayed;
            counter.visibilityTimeout = queue.attributes.visibilityTimeout;
            counter.maxMessageSize = queue.attributes.maxMessageSize;
            counter.retentionPeriod = queue.attributes.messageRetentionPeriod;
            counter.size = queue.size;
            counter.isDlq = queue.isDlq;
            counter.created = queue.created;
            counter.modified = queue.modified;
            listQueueCountersResponse.queueCounters.emplace_back(counter);
        }
        return listQueueCountersResponse;
    }

    Message Mapper::map(const Database::Entity::SQS::Message &messageEntity) {

        Message messageDto;
        messageDto.id = messageEntity.oid;
        messageDto.queueName = messageEntity.queueName;
        messageDto.queueArn = messageEntity.queueArn;
        messageDto.queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(messageEntity.queueArn);
        messageDto.messageId = messageEntity.messageId;
        messageDto.receiptHandle = messageEntity.receiptHandle;
        messageDto.body = messageEntity.body;
        messageDto.size = messageEntity.size;
        messageDto.attributes = messageEntity.attributes;
        messageDto.messageAttributes = map(messageEntity.messageAttributes);
        messageDto.md5OfBody = Database::SqsUtils::CreateMd5OfMessageBody(messageEntity.body);
        messageDto.md5OfMessageAttributes = Database::SqsUtils::CreateMd5OfMessageAttributes(messageEntity.messageAttributes);
        return messageDto;
    }

    std::vector<Message> Mapper::map(const std::vector<Database::Entity::SQS::Message> &messageEntities) {

        std::vector<Message> messageDtos;
        for (const auto &messageEntity: messageEntities) {
            messageDtos.emplace_back(map(messageEntity));
        }
        return messageDtos;
    }

    Database::Entity::SQS::MessageAttributeList Mapper::map(const std::map<std::string, MessageAttribute> &messageAttributes) {
        Database::Entity::SQS::MessageAttributeList messageAttributeList{};
        for (const auto &[fst, snd]: messageAttributes) {
            Database::Entity::SQS::MessageAttribute messageAttribute;
            messageAttribute.stringValue = fst;
            messageAttribute.stringListValues = snd.stringListValues;
            messageAttribute.dataType = Database::Entity::SQS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(snd.dataType));
            messageAttributeList[fst] = messageAttribute;
        }
        return messageAttributeList;
    }

    std::map<std::string, MessageAttribute> Mapper::map(const std::map<std::string, Database::Entity::SQS::MessageAttribute> &messageAttributes) {
        std::map<std::string, MessageAttribute> messageAttributeList{};
        if (!messageAttributes.empty()) {
            for (const auto &[fst, snd]: messageAttributes) {
                MessageAttribute messageAttribute;
                messageAttribute.dataType = MessageAttributeDataTypeFromString(Database::Entity::SQS::MessageAttributeTypeToString(snd.dataType));
                messageAttribute.stringValue = snd.stringValue;
                messageAttribute.stringListValues = snd.stringListValues;
                messageAttributeList[fst] = messageAttribute;
            }
        }
        return messageAttributeList;
    }

    std::map<std::string, EventMessageAttribute> Mapper::mapEventMessageAttributes(const std::map<std::string, Database::Entity::SQS::MessageAttribute> &messageAttributes) {
        std::map<std::string, EventMessageAttribute> messageAttributeList{};
        if (!messageAttributes.empty()) {
            for (const auto &[fst, snd]: messageAttributes) {
                EventMessageAttribute messageAttribute;
                messageAttribute.dataType = MessageAttributeDataTypeFromString(Database::Entity::SQS::MessageAttributeTypeToString(snd.dataType));
                messageAttribute.stringValue = snd.stringValue;
                messageAttribute.stringListValues = snd.stringListValues;
                messageAttributeList[fst] = messageAttribute;
            }
        }
        return messageAttributeList;
    }
}// namespace AwsMock::Dto::SQS