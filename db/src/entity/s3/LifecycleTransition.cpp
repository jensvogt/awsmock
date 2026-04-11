//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/LifecycleTransition.h>

namespace AwsMock::Database::Entity::S3 {

    LifecycleTransition::LifecycleTransition(const view &mResult) {
        FromDocument(mResult);
    }

    view_or_value<view, value> LifecycleTransition::ToDocument() const {

        auto transitionDoc = document{};
        transitionDoc.append(kvp("days", days));
        transitionDoc.append(kvp("date", bsoncxx::types::b_date(date)));
        transitionDoc.append(kvp("storageClass", StorageClassToString(storageClass)));

        return transitionDoc.extract();
    }

    LifecycleTransition LifecycleTransition::FromDocument(const view &mResult) {
        LifecycleTransition t;
        t.days = Core::Bson::BsonUtils::GetIntValue(mResult["days"]);
        t.date = Core::Bson::BsonUtils::GetDateValue(mResult["date"]);
        t.storageClass = StorageClassFromString(Core::Bson::BsonUtils::GetStringValue(mResult["storageClass"]));
        return t;
    }

} // namespace AwsMock::Database::Entity::S3
