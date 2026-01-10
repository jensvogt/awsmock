//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/LifecycleTransition.h>

namespace AwsMock::Database::Entity::S3 {

    LifecycleTransition::LifecycleTransition(const view &mResult) {
        FromDocument(mResult);
    }

    view_or_value<view, value> LifecycleTransition::ToDocument() const {

        auto filterRuleDoc = document{};
        filterRuleDoc.append(kvp("days", days));
        filterRuleDoc.append(kvp("date", bsoncxx::types::b_date(date)));
        filterRuleDoc.append(kvp("storageClass", StorageClassToString(storeClass)));

        return filterRuleDoc.extract();
    }

    void LifecycleTransition::FromDocument(const view &mResult) {

        days = Core::Bson::BsonUtils::GetIntValue(mResult["id"]);
        date = Core::Bson::BsonUtils::GetDateValue(mResult["id"]);
        storeClass = StorageClassFromString(Core::Bson::BsonUtils::GetStringValue(mResult["storeClass"]));
    }

}// namespace AwsMock::Database::Entity::S3
