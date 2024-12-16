//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/sqs/SendMessageRequest.h>

namespace AwsMock::Dto::SQS {

    void SendMessageRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            queueUrl = Core::Bson::BsonUtils::GetStringValue(document, "QueueUrl");
            body = Core::Bson::BsonUtils::GetStringValue(document, "MessageBody");

            // Sanitize
            queueUrl = Core::SanitizeSQSUrl(queueUrl);

            // Attributes
            if (document.view().find("MessageAttributes") != document.view().end()) {
                for (const view attributesView = document.view()["MessageAttributes"].get_document().value; const bsoncxx::document::element &attributeElement: attributesView) {
                    MessageAttribute attribute;
                    std::string key = bsoncxx::string::to_string(attributeElement.key());
                    attribute.FromDocument(attributeElement.get_document().value);
                    messageAttributes[key] = attribute;
                }
            }

            // System attributes
            if (document.view().find("MessageSystemAttributes") != document.view().end()) {
                for (const view attributesView = document.view()["MessageSystemAttributes"].get_document().value; const bsoncxx::document::element &attributeElement: attributesView) {
                    std::string key = bsoncxx::string::to_string(attributeElement.key());
                    const std::string value = bsoncxx::string::to_string(attributesView[key].get_string().value);
                    attributes[key] = value;
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string SendMessageRequest::ToJson() {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "QueueUrl", queueUrl);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "MessageBody", body);

            if (!messageAttributes.empty()) {
                array jsonMessageAttributeArray;
                for (const auto &[fst, snd]: messageAttributes) {
                    document jsonAttribute;
                    jsonAttribute.append(kvp(fst, snd.ToDocument()));
                    jsonMessageAttributeArray.append(jsonAttribute);
                }
                rootDocument.append(kvp("messageAttributes", jsonMessageAttributeArray));
            }

            if (!attributes.empty()) {
                document jsonAttributeObject;
                for (const auto &[fst, snd]: attributes) {
                    jsonAttributeObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("attributes", jsonAttributeObject));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    std::string SendMessageRequest::ToString() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, SendMessageRequest &r) {
        os << "SendMessageRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
