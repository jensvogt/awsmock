//
// Created by vogje01 on 7/6/24.
//

#include <awsmock/dto/sqs/model/MessageEntry.h>

namespace AwsMock::Dto::SQS {

    std::string MessageEntry::ToJson() const {

        try {

            return Core::Bson::BsonUtils::ToJsonString(ToDocument());

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> MessageEntry::ToDocument() const {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "id", id);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "messageId", messageId);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "receiptHandle", receiptHandle);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "body", body);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5OfBody", md5Sum);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "retries", retries);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "size", size);
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "created", created);
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "modified", modified);

            // Message attributes
            if (!messageAttributes.empty()) {
                array jsonMessageAttributeArray;
                for (const auto &[fst, snd]: messageAttributes) {
                    document jsonAttribute;
                    // TODO: Fix me
                    //jsonAttribute.append(kvp(fst, snd.ToDocument()));
                    jsonMessageAttributeArray.append(jsonAttribute);
                }
                rootDocument.append(kvp("messageAttributes", jsonMessageAttributeArray));
            }

            // System attributes
            if (!attributes.empty()) {
                array jsonAttributeArray;
                for (const auto &[fst, snd]: attributes) {
                    document jsonAttribute;
                    jsonAttribute.append(kvp(fst, snd));
                    jsonAttributeArray.append(jsonAttribute);
                }
                rootDocument.append(kvp("attributes", jsonAttributeArray));
            }
            return rootDocument.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void MessageEntry::FromDocument(const view_or_value<view, value> &object) {

        try {
            region = Core::Bson::BsonUtils::GetStringValue(object, "Region");
            id = Core::Bson::BsonUtils::GetStringValue(object, "Id");
            body = Core::Bson::BsonUtils::GetStringValue(object, "MessageBody");

            // Attributes
            if (object.view().find("MessageAttributes") != object.view().end()) {
                for (const bsoncxx::array::view attributesView{object.view()["MessageAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                    MessageAttribute attribute;
                    std::string key = bsoncxx::string::to_string(attributeElement.key());
                    // TODO: Fix me
                    //attribute.FromDocument(attributeElement.get_document().value);
                    messageAttributes[key] = attribute;
                }
            }

            // System attributes
            if (object.view().find("MessageSystemAttributes") != object.view().end()) {

                for (const bsoncxx::array::view attributesView{object.view()["MessageSystemAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                    std::string key = bsoncxx::string::to_string(attributeElement.key());
                    const std::string value = bsoncxx::string::to_string(attributeElement.get_string().value);
                    attributes[key] = value;
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string MessageEntry::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MessageEntry &r) {
        os << "MessageEntry=" + r.ToJson();
        return os;
    }
};// namespace AwsMock::Dto::SQS
