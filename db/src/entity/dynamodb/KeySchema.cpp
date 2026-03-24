//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/entity/dynamodb/KeySchema.h>

#include "awsmock/dto/cognito/AdminCreateUserRequest.h"

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> KeySchema::ToDocument() const {

        auto attributeDoc = document{};
        Core::Bson::BsonUtils::SetStringValue(attributeDoc, "attributeName", attributeName);
        Core::Bson::BsonUtils::SetStringValue(attributeDoc, "keyType", keyType);
        return attributeDoc.extract();
    }

    void KeySchema::FromDocument(view_or_value<view, value> mResult) {
        attributeName = Core::Bson::BsonUtils::GetStringValue(mResult, "attributeName");
        keyType = Core::Bson::BsonUtils::GetStringValue(mResult, "keyType");
    }

}// namespace AwsMock::Database::Entity::DynamoDb
