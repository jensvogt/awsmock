//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/sns/Message.h>

namespace AwsMock::Database::Entity::SNS {

    view_or_value<view, value> Message::ToDocument() const {

        auto messageAttributesDoc = array{};
        for (const auto &messageAttribute: messageAttributes) {
            messageAttributesDoc.append(messageAttribute.ToDocument());
        }

        // Mandatory fields
        document messageDoc;
        messageDoc.append(kvp("region", region),
                          kvp("topicArn", topicArn),
                          kvp("targetArn", targetArn),
                          kvp("message", message),
                          kvp("messageId", messageId),
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
            lastSend = Core::Bson::BsonUtils::GetDateValue(mResult, "lastSend");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            if (mResult.value().find("messageAttributes") != mResult.value().end()) {
                if (const bsoncxx::array::view attributesView{mResult.value()["messageAttributes"].get_array().value}; !attributesView.empty()) {
                    for (const bsoncxx::array::element &attributeElement: attributesView) {
                        MessageAttribute attribute{
                                .attributeName = bsoncxx::string::to_string(attributeElement["attributeName"].get_string().value),
                                .attributeValue = bsoncxx::string::to_string(attributeElement["attributeValue"].get_string().value)};
                        messageAttributes.push_back(attribute);
                    }
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
