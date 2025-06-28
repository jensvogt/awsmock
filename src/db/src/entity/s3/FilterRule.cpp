//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/FilterRule.h>

namespace AwsMock::Database::Entity::S3 {

    view_or_value<view, value> FilterRule::ToDocument() const {

        auto filterRuleDoc = document{};
        filterRuleDoc.append(kvp("name", name));
        filterRuleDoc.append(kvp("value", value));

        return filterRuleDoc.extract();
    }

    void FilterRule::FromDocument(const view &mResult) {

        name = Core::Bson::BsonUtils::GetStringValue(mResult["name"]);
        value = Core::Bson::BsonUtils::GetStringValue(mResult["value"]);
    }

}// namespace AwsMock::Database::Entity::S3
