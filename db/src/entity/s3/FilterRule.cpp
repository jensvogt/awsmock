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

    FilterRule FilterRule::FromDocument(const view &mResult) {

        FilterRule f;
        f.name = Core::Bson::BsonUtils::GetStringValue(mResult["name"]);
        f.value = Core::Bson::BsonUtils::GetStringValue(mResult["value"]);
        return f;
    }

} // namespace AwsMock::Database::Entity::S3
