//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/QueueNotification.h>

namespace AwsMock::Database::Entity::S3 {

    bool QueueNotification::CheckFilter(const std::string &key) {

        if (filterRules.empty()) {
            return true;
        }

        return std::ranges::any_of(filterRules, [key](const FilterRule &rule) {
            return (rule.name == "prefix" && key.starts_with(rule.value)) || (rule.name == "suffix" && key.ends_with(rule.value));
        });
    }

    view_or_value<view, value> QueueNotification::ToDocument() const {

        auto queueNotificationDoc = document{};
        queueNotificationDoc.append(kvp("id", id));
        queueNotificationDoc.append(kvp("queueArn", queueArn));

        // Events
        auto eventsDoc = bsoncxx::builder::basic::array{};
        for (const auto &event: events) {
            eventsDoc.append(event);
        }
        queueNotificationDoc.append(kvp("events", eventsDoc));

        auto filterRulesDoc = array{};
        for (const auto &filterRule: filterRules) {
            filterRulesDoc.append(filterRule.ToDocument());
        }
        queueNotificationDoc.append(kvp("filterRules", filterRulesDoc));

        return queueNotificationDoc.extract();
    }

    QueueNotification QueueNotification::FromDocument(const std::optional<view> &mResult) {

        try {

            id = Core::Bson::BsonUtils::GetStringValue(mResult.value()["id"]);
            queueArn = Core::Bson::BsonUtils::GetStringValue(mResult.value()["queueArn"]);

            // Extract events
            if (mResult.value().find("events") != mResult.value().end()) {
                for (const view eventsView = mResult.value()["events"].get_array().value; const bsoncxx::document::element &eventElement: eventsView) {
                    events.emplace_back(eventElement.get_string().value);
                }
            }

            // Extract filter rules
            if (mResult.value().find("filterRules") != mResult.value().end()) {
                for (const view filterRulesView = mResult.value()["filterRules"].get_array().value; const bsoncxx::document::element &filterRuleElement: filterRulesView) {
                    FilterRule filterRule;
                    filterRule.FromDocument(filterRuleElement.get_document().view());
                    filterRules.emplace_back(filterRule);
                }
            }

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::DatabaseException(exc.what());
        }
        return *this;
    }

}// namespace AwsMock::Database::Entity::S3
