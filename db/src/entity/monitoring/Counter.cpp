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
                kvp("timestamp", bsoncxx::types::b_date(timestamp)));
        return counterDoc;
    }

    void Counter::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        labelName = Core::Bson::BsonUtils::GetStringValue(mResult, "labelName");
        labelValue = Core::Bson::BsonUtils::GetStringValue(mResult, "labelValue");
        performanceValue = Core::Bson::BsonUtils::GetDoubleValue(mResult, "value");
        timestamp = bsoncxx::types::b_date(mResult.value()["timestamp"].get_date().value);
    }

    std::string Counter::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Counter &counter) {
        os << "Counter=" << to_json(counter.ToDocument());
        return os;
    }
}// namespace AwsMock::Database::Entity::Monitoring