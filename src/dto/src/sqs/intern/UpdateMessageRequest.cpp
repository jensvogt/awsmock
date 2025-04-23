//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/sqs/internal/UpdateMessageRequest.h>

namespace AwsMock::Dto::SQS {

    void UpdateMessageRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            messageId = Core::Bson::BsonUtils::GetStringValue(document, "MessageId");

            // Attributes
            if (document.view().find("MessageAttributes") != document.view().end()) {
                for (const view attributesView = document.view()["MessageAttributes"].get_array().value; const bsoncxx::document::element &attributeElement: attributesView) {
                    MessageAttribute attribute;
                    std::string key = bsoncxx::string::to_string(attributeElement.key());
                    attribute.FromDocument(attributeElement.get_document().value);
                    messageAttributes[key] = attribute;
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateMessageRequest::ToJson() const {

        try {
            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "MessageId", messageId);

            if (!messageAttributes.empty()) {
                array jsonMessageAttributeArray;
                for (const auto &[fst, snd]: messageAttributes) {
                    document jsonAttribute;
                    jsonAttribute.append(kvp(fst, snd.ToDocument()));
                    jsonMessageAttributeArray.append(jsonAttribute);
                }
                rootDocument.append(kvp("messageAttributes", jsonMessageAttributeArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    std::string UpdateMessageRequest::ToString() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, UpdateMessageRequest &r) {
        os << "UpdateMessageRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
