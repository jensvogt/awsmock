//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/lambda/EventSourceMapping.h>

namespace AwsMock::Database::Entity::Lambda {

    void EventSourceMapping::FromDocument(const std::optional<view> &mResult) {

        type = Core::Bson::BsonUtils::GetStringValue(mResult, "type");
        eventSourceArn = Core::Bson::BsonUtils::GetStringValue(mResult, "eventSourceArn");
        batchSize = Core::Bson::BsonUtils::GetIntValue(mResult, "batchSize");
        maximumBatchingWindowInSeconds = Core::Bson::BsonUtils::GetIntValue(mResult, "maximumBatchingWindowInSeconds");
        enabled = Core::Bson::BsonUtils::GetBoolValue(mResult, "enabled");
        uuid = Core::Bson::BsonUtils::GetStringValue(mResult, "uuid");
    }

    view_or_value<view, value> EventSourceMapping::ToDocument() const {

        document eventSourceMappingDoc{};
        eventSourceMappingDoc.append(kvp("type", type));
        eventSourceMappingDoc.append(kvp("eventSourceArn", eventSourceArn));
        eventSourceMappingDoc.append(kvp("batchSize", bsoncxx::types::b_int64(batchSize)));
        eventSourceMappingDoc.append(kvp("maximumBatchingWindowInSeconds", bsoncxx::types::b_int64(maximumBatchingWindowInSeconds)));
        eventSourceMappingDoc.append(kvp("enabled", enabled));
        eventSourceMappingDoc.append(kvp("uuid", uuid));
        return eventSourceMappingDoc.extract();
    }

    [[nodiscard]] std::string EventSourceMapping::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const EventSourceMapping &t) {
        os << "EventSourceMapping=" << to_json(t.ToDocument());
        return os;
    }
}// namespace AwsMock::Database::Entity::Lambda