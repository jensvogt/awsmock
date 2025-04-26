//
// Created by vogje01 on 24/09/2023.
//

#include <awsmock/entity/sqs/MessageAttribute.h>

namespace AwsMock::Database::Entity::SQS {

    view_or_value<view, value> MessageAttribute::ToDocument() const {

        view_or_value<view, value> messageAttributeDoc = make_document(
                kvp("attributeName", attributeName),
                kvp("attributeValue", attributeValue),
                kvp("attributeType", MessageAttributeTypeToString(attributeType)));

        return messageAttributeDoc;
    }

    void MessageAttribute::FromDocument(const view_or_value<view, value> &object) {

        try {

            attributeName = Core::Bson::BsonUtils::GetStringValue(object, "attributeName");
            attributeValue = Core::Bson::BsonUtils::GetStringValue(object, "attributeValue");
            attributeType = MessageAttributeTypeFromString(Core::Bson::BsonUtils::GetStringValue(object, "attributeType"));

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::SQS
