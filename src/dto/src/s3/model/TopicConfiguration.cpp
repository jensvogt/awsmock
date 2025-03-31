//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/dto/s3/model/TopicConfiguration.h>

namespace AwsMock::Dto::S3 {

    void TopicConfiguration::FromDocument(const view_or_value<view, value> &document) {

        try {

            id = Core::Bson::BsonUtils::GetStringValue(document, "Id");
            topicArn = Core::Bson::BsonUtils::GetStringValue(document, "Topic");

            // Events
            if (document.view().find("events") != document.view().end()) {
                for (const view eventsView = document.view()["events"].get_array().value; bsoncxx::document::element event: eventsView) {
                    events.emplace_back(EventTypeFromString(std::string(event.get_string().value)));
                }
            }

            // Filter rules
            if (document.view().find("filterRules") != document.view().end()) {
                for (const view filterRuleView = document.view()["filterRules"].get_array().value; bsoncxx::document::element element: filterRuleView) {
                    FilterRule filterRule;
                    filterRule.FromDocument(element.get_document().view());
                    filterRules.emplace_back(filterRule);
                }
            }

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    view_or_value<view, value> TopicConfiguration::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Id", id);
            Core::Bson::BsonUtils::SetStringValue(document, "Topic", topicArn);

            // Filters
            if (!filterRules.empty()) {
                array jsonArray;
                for (const auto &filter: filterRules) {
                    jsonArray.append(filter.ToDocument());
                }
                document.append(kvp("Filter", jsonArray));
            }

            // Event
            if (!events.empty()) {
                array jsonArray;
                for (const auto &event: events) {
                    jsonArray.append(EventTypeToString(event));
                }
                document.append(kvp("Event", jsonArray));
            }
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void TopicConfiguration::FromXml(const boost::property_tree::ptree &pt) {
        if (pt.get_optional<std::string>("Id")) {
            id = pt.get<std::string>("Id");
        }
        if (pt.get_optional<std::string>("Topic")) {
            topicArn = pt.get<std::string>("Topic");
        }
        if (pt.get_optional<std::string>("Filter")) {
            FilterRule filter;
            filter.FromXml(pt.get_child("Filter"));
            filterRules.emplace_back(filter);
        }
        if (pt.get_optional<std::string>("Event")) {
            events.emplace_back(EventTypeFromString(pt.get<std::string>("Event")));
        }
    }

    std::string TopicConfiguration::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string TopicConfiguration::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const TopicConfiguration &r) {
        os << "TopicConfiguration=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
