//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/LambdaNotification.h>

namespace AwsMock::Database::Entity::S3 {

    bool LambdaNotification::CheckFilter(const std::string &key) {

        if (filterRules.empty()) {
            return true;
        }

        return std::ranges::any_of(filterRules, [key](const FilterRule &rule) {
            return (rule.name == "prefix" && key.starts_with(rule.value)) || (rule.name == "suffix" && key.ends_with(rule.value));
        });
    }

    view_or_value<view, value> LambdaNotification::ToDocument() const {

        auto lambdaNotificationDoc = document{};
        lambdaNotificationDoc.append(kvp("id", id));
        lambdaNotificationDoc.append(kvp("lambdaArn", lambdaArn));

        // Events
        auto eventsDoc = array{};
        for (const auto &event: events) {
            eventsDoc.append(event);
        }
        lambdaNotificationDoc.append(kvp("events", eventsDoc));

        // Filter rules
        auto filterRulesDoc = bsoncxx::builder::basic::array{};
        for (const auto &filterRule: filterRules) {
            filterRulesDoc.append(filterRule.ToDocument());
        }
        lambdaNotificationDoc.append(kvp("filterRules", filterRulesDoc));

        return lambdaNotificationDoc.extract();
    }

    LambdaNotification LambdaNotification::FromDocument(const std::optional<view> &mResult) {

        try {

            id = Core::Bson::BsonUtils::GetStringValue(mResult.value()["id"]);
            lambdaArn = Core::Bson::BsonUtils::GetStringValue(mResult.value()["lambdaArn"]);

            // Extract events
            if (mResult.value().find("events") != mResult.value().end()) {
                for (const view eventsView = mResult.value()["events"].get_array().value; bsoncxx::document::element event: eventsView) {
                    events.emplace_back(event.get_string().value);
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
            throw Core::JsonException(exc.what());
        }
        return *this;
    }

}// namespace AwsMock::Database::Entity::S3
