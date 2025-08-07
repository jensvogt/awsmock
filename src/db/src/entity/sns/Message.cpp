//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/sns/Message.h>

namespace AwsMock::Database::Entity::SNS {

    view_or_value<view, value> Message::ToDocument() const {

        document messageAttributesDoc;
        for (const auto &[fst, snd]: messageAttributes) {
            messageAttributesDoc.append(kvp(fst, snd.ToDocument()));
        }

        // Mandatory fields
        document messageDoc;
        messageDoc.append(kvp("region", region),
                          kvp("topicArn", topicArn),
                          kvp("targetArn", targetArn),
                          kvp("message", message),
                          kvp("messageId", messageId),
                          kvp("contentType", contentType),
                          kvp("size", bsoncxx::types::b_int64(size)),
                          kvp("status", MessageStatusToString(status)),
                          kvp("messageAttribute", messageAttributesDoc));
        MongoUtils::SetDatetime(messageDoc, "lastSend", lastSend);
        MongoUtils::SetDatetime(messageDoc, "created", created);
        MongoUtils::SetDatetime(messageDoc, "modified", modified);
        return messageDoc.extract();
    }

    void Message::FromDocument(const std::optional<view> &mResult) {

        try {
            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            topicArn = Core::Bson::BsonUtils::GetStringValue(mResult, "topicArn");
            targetArn = Core::Bson::BsonUtils::GetStringValue(mResult, "targetArn");
            message = Core::Bson::BsonUtils::GetStringValue(mResult, "message");
            status = MessageStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "status"));
            messageId = Core::Bson::BsonUtils::GetStringValue(mResult, "messageId");
            size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
            contentType = Core::Bson::BsonUtils::GetStringValue(mResult, "contentType");
            lastSend = Core::Bson::BsonUtils::GetDateValue(mResult, "lastSend");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Attributes
            if (mResult.value().find("messageAttributes") != mResult.value().end()) {
                messageAttributes.clear();
                for (const view messageAttributeObject = mResult.value()["messageAttributes"].get_document().value; const auto &a: messageAttributeObject) {
                    MessageAttribute attribute;
                    std::string key = bsoncxx::string::to_string(a.key());
                    attribute.FromDocument(a.get_document().value);
                    messageAttributes[key] = attribute;
                }
            }
        } catch (std::exception &exc) {
            log_error << "SNS message exception: " << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Message::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Message::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Message &message) {
        os << "Message=" << to_json(message.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SNS
