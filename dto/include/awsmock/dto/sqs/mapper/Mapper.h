//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_SQS_MAPPER_H
#define AWSMOCK_DTO_SQS_MAPPER_H

// AwsMock includes
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/sqs/CreateQueueRequest.h>
#include <awsmock/dto/sqs/CreateQueueResponse.h>
#include <awsmock/dto/sqs/SendMessageRequest.h>
#include <awsmock/dto/sqs/SendMessageResponse.h>
#include <awsmock/dto/sqs/internal/ListMessageCountersResponse.h>
#include <awsmock/dto/sqs/internal/ListQueueCountersResponse.h>
#include <awsmock/dto/sqs/model/AttributeCounter.h>
#include <awsmock/dto/sqs/model/EventMessageAttribute.h>
#include <awsmock/dto/sqs/model/Message.h>
#include <awsmock/dto/sqs/model/MessageEntry.h>
#include <awsmock/dto/sqs/model/Queue.h>
#include <awsmock/dto/sns/model/MessageAttribute.h>
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

    class CreateQueueRequestMapper : public StaticMapper<CreateQueueRequestMapper, Database::Entity::SQS::Queue, CreateQueueRequest> {

    public:
        static Database::Entity::SQS::Queue toEntity(const CreateQueueRequest &d) {
            Database::Entity::SQS::Queue e;
            e.region = d.region;
            e.name = d.queueName;
            e.owner = d.user;
            e.arn = Core::CreateSQSQueueArn(d.queueName);
            e.url = Core::CreateSQSQueueUrl(d.queueName);
            for (const auto &[fst, snd]: d.attributes) {
                if (fst == "QueueArn") e.attributes.queueArn = snd;
                if (fst == "DelaySeconds") e.attributes.delaySeconds = std::stol(snd);
                if (fst == "MaxMessageSize") e.attributes.maxMessageSize = std::stol(snd);
                if (fst == "MessageRetentionPeriod") e.attributes.messageRetentionPeriod = std::stol(snd);
                if (fst == "ReceiveMessageWaitTime") e.attributes.receiveMessageWaitTime = std::stol(snd);
                if (fst == "VisibilityTimeout") e.attributes.visibilityTimeout = std::stol(snd);
                if (fst == "Policy") e.attributes.policy = snd;
                if (fst == "RedriveAllowPolicy") e.attributes.redriveAllowPolicy = snd;
            }
            e.tags = d.tags;
            return e;
        }
    };

    class CreateQueueResponseMapper : public StaticMapper<CreateQueueResponseMapper, Database::Entity::SQS::Queue, CreateQueueResponse> {

    public:
        static CreateQueueResponse toDto(const Database::Entity::SQS::Queue &e) {
            CreateQueueResponse d;
            d.region = e.region;
            d.owner = e.owner;
            d.queueName = e.name;
            d.queueArn = e.arn;
            d.queueUrl = e.url;
            return d;
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

    class QueueCounterMapper : public StaticMapper<QueueCounterMapper, Database::Entity::SQS::Queue, QueueCounter> {

    public:
        static QueueCounter toDto(const Database::Entity::SQS::Queue &e) {
            QueueCounter d;
            d.region = e.region;
            d.queueName = e.name;
            d.queueArn = e.arn;
            d.queueUrl = e.url;
            d.size = e.size;
            d.available = e.attributes.approximateNumberOfMessages;
            d.invisible = e.attributes.approximateNumberOfMessagesNotVisible;
            d.delayed = e.attributes.approximateNumberOfMessagesDelayed;
            d.visibilityTimeout = e.attributes.visibilityTimeout;
            d.maxMessageSize = e.attributes.maxMessageSize;
            d.delay = e.attributes.delaySeconds;
            d.retentionPeriod = e.attributes.messageRetentionPeriod;
            d.created = e.created;
            d.modified = e.modified;
            return d;
        }
    };

    class MessageAttributeMapper : public StaticMapper<MessageAttributeMapper, Database::Entity::SQS::MessageAttribute, MessageAttribute> {
    public:
        static MessageAttribute toDto(const Database::Entity::SQS::MessageAttribute &e) {
            MessageAttribute messageAttribute;
            messageAttribute.dataType = MessageAttributeDataTypeFromString(Database::Entity::SQS::MessageAttributeTypeToString(e.dataType));
            messageAttribute.stringValue = e.stringValue;
            messageAttribute.stringListValues = e.stringListValues;
            return messageAttribute;
        }

        static Database::Entity::SQS::MessageAttribute toEntity(const MessageAttribute &d) {
            Database::Entity::SQS::MessageAttribute messageAttribute;
            messageAttribute.dataType = Database::Entity::SQS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(d.dataType));
            messageAttribute.stringValue = d.stringValue;
            messageAttribute.stringListValues = d.stringListValues;
            return messageAttribute;
        }
    };

    class SNSMessageAttributeMapper : public StaticMapper<SNSMessageAttributeMapper, SNS::MessageAttribute, MessageAttribute> {
    public:
        static MessageAttribute toDto(const SNS::MessageAttribute &e) {
            MessageAttribute messageAttribute;
            messageAttribute.dataType = MessageAttributeDataTypeFromString(SNS::MessageAttributeDataTypeToString(e.dataType));
            messageAttribute.stringValue = e.stringValue;
            messageAttribute.stringListValues = e.stringListValues;
            messageAttribute.binaryValue = e.binaryValue;
            messageAttribute.binaryListValues = e.binaryListValues;
            return messageAttribute;
        }

        static SNS::MessageAttribute toEntity(const MessageAttribute &d) {
            SNS::MessageAttribute messageAttribute;
            messageAttribute.dataType = SNS::MessageAttributeDataTypeFromString(MessageAttributeDataTypeToString(d.dataType));
            messageAttribute.stringValue = d.stringValue;
            messageAttribute.stringListValues = d.stringListValues;
            messageAttribute.binaryValue = d.binaryValue;
            messageAttribute.binaryListValues = d.binaryListValues;
            return messageAttribute;
        }
    };

    class MessageMapper : public StaticMapper<MessageMapper, Database::Entity::SQS::Message, Message> {

    public:
        static Message toDto(const Database::Entity::SQS::Message &e) {
            Message d;
            d.queueName = e.queueName;
            d.queueArn = e.queueArn;
            d.queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(e.queueArn);
            d.messageId = e.messageId;
            d.receiptHandle = e.receiptHandle;
            d.body = e.body;
            d.size = e.size;
            d.attributes = e.attributes;
            d.messageAttributes = MessageAttributeMapper::toDtoMap(e.messageAttributes);
            d.md5OfBody = Database::SqsUtils::CreateMd5OfMessageBody(e.body);
            d.md5OfMessageAttributes = Database::SqsUtils::CreateMd5OfMessageAttributes(e.messageAttributes);
            return d;
        }

        static Database::Entity::SQS::Message toEntity(const Message &d) {
            Database::Entity::SQS::Message e;
            e.queueName = d.queueName;
            e.queueArn = d.queueArn;
            e.messageId = d.messageId;
            e.receiptHandle = d.receiptHandle;
            e.body = d.body;
            e.size = d.size;
            e.attributes = d.attributes;
            e.messageAttributes = MessageAttributeMapper::toEntityMap(d.messageAttributes);
            return e;
        }
    };

    class MessageCounterMapper : public StaticMapper<MessageCounterMapper, Database::Entity::SQS::Message, MessageCounter> {

    public:
        static MessageCounter toDto(const Database::Entity::SQS::Message &e) {
            MessageCounter d;
            d.queueName = e.queueName;
            d.queueArn = e.queueArn;
            d.queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(e.queueArn);
            d.messageId = e.messageId;
            d.receiptHandle = e.receiptHandle;
            d.contentType = e.contentType;
            d.body = e.body;
            d.size = e.size;
            d.attributes = e.attributes;
            d.md5OfBody = e.md5Body;
            d.md5OfMessageAttributes = e.md5MessageAttributes;
            d.md5OfSystemAttributes = e.md5MessageSystemAttributes;
            d.created = e.created;
            d.modified = e.modified;
            d.messageAttributes = MessageAttributeMapper::toDtoMap(e.messageAttributes);
            return d;
        }

        static Database::Entity::SQS::Message toEntity(const MessageCounter &d) = delete;
    };

    class SendMessageRequestMapper : public StaticMapper<SendMessageRequestMapper, Database::Entity::SQS::Message, Message> {
    public:
        static Database::Entity::SQS::Message toEntity(const SendMessageRequest &d) {
            Database::Entity::SQS::Message e;
            e.queueName = Core::AwsUtils::ConvertSQSQueueUrlToName(d.queueUrl);
            e.queueArn = Core::AwsUtils::CreateSQSQueueArn(d.region, Core::Configuration::instance().GetAccountId(), e.queueName);
            e.body = d.body;
            e.attributes = d.attributes;
            e.messageAttributes = MessageAttributeMapper::toEntityMap(d.messageAttributes);
            e.md5Body = Database::SqsUtils::CreateMd5OfMessageBody(d.body);
            e.md5MessageAttributes = Database::SqsUtils::CreateMd5OfMessageAttributes(e.messageAttributes);
            e.md5MessageSystemAttributes = Database::SqsUtils::CreateMd5OfMessageSystemAttributes(d.attributes);
            return e;
        }

        static Database::Entity::SQS::Message toDto(const SendMessageRequest &d) = delete;
    };

    // class SendNotificationMessageRequestMapper : public StaticMapper<SendMessageRequestMapper, Database::Entity::SQS::Message, SendNotificationMessageRequest> {
    // public:
    //     static Database::Entity::SQS::Message toEntity(const SendNotificationMessageRequest &d) {
    //         Database::Entity::SQS::Message e;
    //         e.queueName = Core::AwsUtils::ConvertSQSQueueUrlToName(d.queueUrl);
    //         e.queueArn = Core::AwsUtils::CreateSQSQueueArn(d.region, Core::Configuration::instance().GetAccountId(), e.queueName);
    //         e.body = d.body;
    //         e.attributes = d.attributes;
    //         e.md5Body = Database::SqsUtils::CreateMd5OfMessageBody(d.body);
    //         e.md5MessageAttributes = Database::SqsUtils::CreateMd5OfMessageAttributes(e.messageAttributes);
    //         e.md5MessageSystemAttributes = Database::SqsUtils::CreateMd5OfMessageSystemAttributes(d.attributes);
    //         for (const auto &a: d.messageAttributes) {
    //             Database::Entity::SQS::MessageAttribute messageAttribute;
    //             messageAttribute.dataType = Database::Entity::SQS::MessageAttributeTypeFromString(MessageAttributeDataTypeToString(a.second.dataType));
    //             if (messageAttribute.dataType == Database::Entity::SQS::STRING) {
    //                 messageAttribute.stringValue = a.second.stringValue;
    //             } else if (messageAttribute.dataType == Database::Entity::SQS::BINARY) {
    //                 messageAttribute.binaryValue = a.second.binaryValue;
    //             }
    //         }
    //         return e;
    //     }
    //
    //     static Database::Entity::SQS::Message toDto(const SendMessageRequest &d) = delete;
    // };

    class SendMessageResponseMapper : public StaticMapper<SendMessageResponseMapper, Database::Entity::SQS::Message, SendMessageResponse> {
    public:
        static SendMessageResponse toDto(const Database::Entity::SQS::Message &e) {
            SendMessageResponse d;
            d.messageId = Core::AwsUtils::ConvertSQSQueueUrlToName(e.messageId);
            d.md5Body = Database::SqsUtils::CreateMd5OfMessageBody(e.body);
            d.md5MessageAttributes = Database::SqsUtils::CreateMd5OfMessageAttributes(e.messageAttributes);
            d.md5MessageSystemAttributes = Database::SqsUtils::CreateMd5OfMessageSystemAttributes(e.attributes);
            return d;
        }

        static Database::Entity::SQS::Message toEntity(const SendMessageResponse &d) = delete;
    };

} // namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MAPPER_H
