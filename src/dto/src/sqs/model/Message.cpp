//
// Created by vogje01 on 7/6/24.
//

#include <awsmock/dto/sqs/model/Message.h>

namespace AwsMock::Dto::SQS {

    std::string Message::ToJson() const {

        try {

            return Core::Bson::BsonUtils::ToJsonString(ToDocument());

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> Message::ToDocument() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "messageId", messageId);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "receiptHandle", receiptHandle);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "body", body);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5OfBody", md5Sum);
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "created", created);
            Core::Bson::BsonUtils::SetDateValue(rootDocument, "modified", modified);

            if (!messageAttributes.empty()) {
                document jsonMessageAttributeObject;
                for (const auto &[fst, snd]: messageAttributes) {
                    // TODO: Fix me
                    //jsonMessageAttributeObject.append(kvp(fst, snd.ToDocument()));
                }
            }

            if (!attributes.empty()) {
                document jsonAttributeObject;
                for (const auto &[fst, snd]: attributes) {
                    jsonAttributeObject.append(kvp(fst, snd));
                }
            }
            return rootDocument.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void Message::FromDocument(const view_or_value<view, value> &document) {

        try {

            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            id = Core::Bson::BsonUtils::GetStringValue(document, "Id");
            body = Core::Bson::BsonUtils::GetStringValue(document, "MessageBody");

            // Attributes
            if (document.view().find("MessageAttributes") != document.view().end()) {
                for (const bsoncxx::array::view attributesView{document.view()["MessageAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                    MessageAttribute attribute;
                    std::string key = bsoncxx::string::to_string(attributeElement.key());
                    // TODO: Fix me
                    //attribute.FromDocument(attributeElement.get_document().value);
                    messageAttributes[key] = attribute;
                }
            }

            // System attributes
            if (document.view().find("MessageSystemAttributes") != document.view().end()) {

                for (const bsoncxx::array::view attributesView{document.view()["MessageSystemAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
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

    [[nodiscard]] std::string Message::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Message &r) {
        os << "Message=" + r.ToJson();
        return os;
    }
};// namespace AwsMock::Dto::SQS
