//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/dynamodb/Tag.h>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> Tag::ToDocument() const {
        auto tagDoc = document{};
        Core::Bson::BsonUtils::SetStringValue(tagDoc, "key", tagKey);
        Core::Bson::BsonUtils::SetStringValue(tagDoc, "value", tagValue);
        return tagDoc.extract();
    }

    Tag Tag::FromDocument(const view_or_value<view, value> &mResult) {

        tagKey = Core::Bson::BsonUtils::GetStringValue(mResult, "key");
        tagValue = Core::Bson::BsonUtils::GetStringValue(mResult, "value");
        return *this;
    }

}// namespace AwsMock::Database::Entity::DynamoDb