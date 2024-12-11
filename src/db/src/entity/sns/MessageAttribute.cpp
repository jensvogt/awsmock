//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/sns/MessageAttribute.h>

namespace AwsMock::Database::Entity::SNS {

    view_or_value<view, value> MessageAttribute::ToDocument() const {

        view_or_value<view, value> messageAttributeDoc = make_document(
                kvp("name", attributeName),
                kvp("value", attributeValue));

        return messageAttributeDoc;
    }

    std::string MessageAttribute::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MessageAttribute &m) {
        os << "MessageAttribute=" << to_json(m.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SNS
