//
// Created by vogje01 on 24/09/2023.
//

#include "awsmock/utils/SqsUtils.h"


#include <awsmock/entity/sqs/Message.h>

namespace AwsMock::Database::Entity::SQS {

    bool Message::HasAttribute(const std::string &key) {
        return std::ranges::find_if(attributes, [key](const std::pair<std::string, std::string> &attribute) {
                   return attribute.first == key;
               }) != attributes.end();
    }

    int Message::GetIntAttribute(const std::string &key) {
        if (HasAttribute(key)) {
            const auto it =
                    std::ranges::find_if(attributes, [key](const std::pair<std::string, std::string> &attribute) {
                        return attribute.first == key;
                    });
            return std::stoi(it->second);
        }
        return -1;
    }

    int Message::RemoveAttribute(const std::string &key) {
        return static_cast<int>(erase_if(attributes, [key](const std::pair<std::string, std::string> &attribute) {
            return attribute.first == key;
        }));
    }

    view_or_value<view, value> Message::ToDocument() const {

        auto messageAttributesDoc = document{};
        for (const auto &[fst, snd]: messageAttributes) {
            messageAttributesDoc.append(kvp(fst, snd.ToDocument()));
        }

        auto attributesDoc = document{};
        for (const auto &[fst, snd]: attributes) {
            attributesDoc.append(kvp(fst, snd));
        }

        auto messageDoc = document{};

        messageDoc.append(kvp("queueArn", queueArn));
        messageDoc.append(kvp("queueName", queueName));
        messageDoc.append(kvp("body", body));
        messageDoc.append(kvp("status", MessageStatusToString(status)));
        messageDoc.append(kvp("retries", bsoncxx::types::b_int64(retries)));
        messageDoc.append(kvp("size", bsoncxx::types::b_int64(size)));
        messageDoc.append(kvp("messageId", messageId));
        messageDoc.append(kvp("receiptHandle", receiptHandle));
        messageDoc.append(kvp("md5Body", md5Body));
        messageDoc.append(kvp("md5MessageAttributes", md5MessageAttributes));
        messageDoc.append(kvp("md5MessageSystemAttributes", md5MessageSystemAttributes));
        messageDoc.append(kvp("contentType", contentType));
        messageDoc.append(kvp("attributes", attributesDoc));
        messageDoc.append(kvp("messageAttributes", messageAttributesDoc));
        MongoUtils::SetDatetime(messageDoc, "reset", reset);
        MongoUtils::SetDatetime(messageDoc, "created", created);
        MongoUtils::SetDatetime(messageDoc, "modified", modified);

        return messageDoc.extract();
    }

    void Message::FromDocument(const std::optional<view> &mResult) {

        try {

            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            queueArn = Core::Bson::BsonUtils::GetStringValue(mResult, "queueArn");
            queueName = Core::Bson::BsonUtils::GetStringValue(mResult, "queueName");
            body = Core::Bson::BsonUtils::GetStringValue(mResult, "body");
            status = MessageStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "status"));
            retries = Core::Bson::BsonUtils::GetIntValue(mResult, "retries");
            messageId = Core::Bson::BsonUtils::GetStringValue(mResult, "messageId");
            receiptHandle = Core::Bson::BsonUtils::GetStringValue(mResult, "receiptHandle");
            md5Body = Core::Bson::BsonUtils::GetStringValue(mResult, "md5Body");
            md5MessageAttributes = Core::Bson::BsonUtils::GetStringValue(mResult, "md5MessageAttributes");
            md5MessageSystemAttributes = Core::Bson::BsonUtils::GetStringValue(mResult, "md5MessageSystemAttributes");
            contentType = Core::Bson::BsonUtils::GetStringValue(mResult, "contentType");
            size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
            reset = MongoUtils::GetDatetime(mResult, "reset");
            created = MongoUtils::GetDatetime(mResult, "created");
            modified = MongoUtils::GetDatetime(mResult, "modified");

            // Message attributes
            if (mResult.value().find("messageAttributes") != mResult.value().end()) {
                messageAttributes.clear();
                for (const view messageAttributeObject = mResult.value()["messageAttributes"].get_document().value; const auto &a: messageAttributeObject) {
                    MessageAttribute attribute;
                    std::string key = bsoncxx::string::to_string(a.key());
                    attribute.FromDocument(a.get_document().value);
                    messageAttributes[key] = attribute;
                }
            }

            // Attributes
            if (mResult.value().find("attributes") != mResult.value().end()) {
                attributes.clear();
                for (const view attributesView = mResult.value()["attributes"].get_document().value; const bsoncxx::document::element &attributeElement: attributesView) {
                    std::string key = bsoncxx::string::to_string(attributeElement.key());
                    std::string value = bsoncxx::string::to_string(attributesView[key].get_string().value);
                    attributes.emplace(key, value);
                }
            }

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::DatabaseException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::SQS
