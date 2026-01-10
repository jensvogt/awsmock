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

    void LifecycleTransition::FromDocument(const view &mResult) {

        days = Core::Bson::BsonUtils::GetIntValue(mResult["days"]);
        date = Core::Bson::BsonUtils::GetDateValue(mResult["date"]);
        storageClass = StorageClassFromString(Core::Bson::BsonUtils::GetStringValue(mResult["storageClass"]));
    }

}// namespace AwsMock::Database::Entity::S3
