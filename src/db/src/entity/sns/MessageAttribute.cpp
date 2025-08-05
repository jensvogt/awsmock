//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/sns/MessageAttribute.h>

namespace AwsMock::Database::Entity::SNS {

    view_or_value<view, value> MessageAttribute::ToDocument() const {

        view_or_value<view, value> messageAttributeDoc = make_document(
                kvp("stringValue", stringValue),
                kvp("dataType", MessageAttributeTypeToString(dataType)));

        return messageAttributeDoc;
    }

    void MessageAttribute::FromDocument(const view_or_value<view, value> &object) {

        try {

            stringValue = Core::Bson::BsonUtils::GetStringValue(object, "stringValue");
            dataType = MessageAttributeTypeFromString(Core::Bson::BsonUtils::GetStringValue(object, "dataType"));

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string MessageAttribute::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string MessageAttribute::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MessageAttribute &m) {
        os << "MessageAttribute=" << m.ToJson();
        return os;
    }

}// namespace AwsMock::Database::Entity::SNS
