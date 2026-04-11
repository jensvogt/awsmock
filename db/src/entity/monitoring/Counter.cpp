//
// Created by vogje01 on 10/2/24.
//


#include <awsmock/entity/monitoring/Counter.h>

namespace AwsMock::Database::Entity::Monitoring {

    view_or_value<view, value> Counter::ToDocument() const {

        view_or_value<view, value> counterDoc = make_document(
            kvp("name", name),
            kvp("labelName", labelName),
            kvp("labelValue", labelValue),
            kvp("value", performanceValue),
            kvp("created", bsoncxx::types::b_date(timestamp)));
        return counterDoc;
    }

    Counter Counter::FromDocument(const std::optional<view> &mResult) {
        Counter c;
        c.oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        c.name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        c.labelName = Core::Bson::BsonUtils::GetStringValue(mResult, "labelName");
        c.labelValue = Core::Bson::BsonUtils::GetStringValue(mResult, "labelValue");
        c.performanceValue = Core::Bson::BsonUtils::GetDoubleValue(mResult, "value");
        c.timestamp = bsoncxx::types::b_date(mResult.value()["created"].get_date().value);
        return c;
    }

} // namespace AwsMock::Database::Entity::Monitoring
