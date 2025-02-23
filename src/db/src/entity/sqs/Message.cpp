//
// Created by vogje01 on 24/09/2023.
//

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
        return -1;
    }

    view_or_value<view, value> Message::ToDocument() const {

        auto messageAttributesDoc = array{};
        for (const auto &messageAttribute: messageAttributes) {
            messageAttributesDoc.append(messageAttribute.ToDocument());
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
        messageDoc.append(kvp("retries", bsoncxx::types::b_int32(retries)));
        messageDoc.append(kvp("size", bsoncxx::types::b_int64(size)));
        messageDoc.append(kvp("messageId", messageId));
        messageDoc.append(kvp("receiptHandle", receiptHandle));
        messageDoc.append(kvp("md5Body", md5Body));
        messageDoc.append(kvp("md5UserAttr", md5UserAttr));
        messageDoc.append(kvp("md5SystemAttr", md5SystemAttr));
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
            md5UserAttr = Core::Bson::BsonUtils::GetStringValue(mResult, "md5UserAttr");
            md5SystemAttr = Core::Bson::BsonUtils::GetStringValue(mResult, "md5SystemAttr");
            contentType = Core::Bson::BsonUtils::GetStringValue(mResult, "contentType");
            size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
            reset = MongoUtils::GetDatetime(mResult, "reset");
            created = MongoUtils::GetDatetime(mResult, "created");
            modified = MongoUtils::GetDatetime(mResult, "modified");

            // Attributes
            if (mResult.value().find("messageAttributes") != mResult.value().end()) {
                for (const bsoncxx::array::view attributesView{mResult.value()["messageAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                    MessageAttribute attribute{
                            .attributeName = bsoncxx::string::to_string(attributeElement["attributeName"].get_string().value),
                            .attributeValue = bsoncxx::string::to_string(attributeElement["attributeValue"].get_string().value),
                            .attributeType = MessageAttributeTypeFromString(bsoncxx::string::to_string(attributeElement["attributeType"].get_string().value)),
                    };
                    messageAttributes.push_back(attribute);
                }
            }

            // Get attributes
            if (mResult.value().find("attributes") != mResult.value().end()) {
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

    std::string Message::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Message::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Message &m) {
        os << "Message=" << to_json(m.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SQS
