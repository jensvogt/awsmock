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

    std::string FilterRule::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const FilterRule &n) {
        os << "FilterRule=" << to_json(n.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::S3
