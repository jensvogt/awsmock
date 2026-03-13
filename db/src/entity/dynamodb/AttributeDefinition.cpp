//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/entity/dynamodb/AttributeDefinition.h>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> AttributeDefinition::ToDocument() const {

        auto attributeDoc = document{};
        Core::Bson::BsonUtils::SetStringValue(attributeDoc, "attributeName", attributeName);
        Core::Bson::BsonUtils::SetStringValue(attributeDoc, "attributeType", attributeType);
        return attributeDoc.extract();
    }

    void AttributeDefinition::FromDocument(view_or_value<view, value> mResult) {
        attributeName = Core::Bson::BsonUtils::GetStringValue(mResult, "attributeName");
        attributeType = Core::Bson::BsonUtils::GetStringValue(mResult, "attributeType");
    }

}// namespace AwsMock::Database::Entity::DynamoDb
