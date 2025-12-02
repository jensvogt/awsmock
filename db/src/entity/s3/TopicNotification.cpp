//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/TopicNotification.h>

namespace AwsMock::Database::Entity::S3 {
    bool TopicNotification::CheckFilter(const std::string &key) {
        if (filterRules.empty()) {
            return true;
        }

        return std::ranges::any_of(filterRules,
                                   [key](const FilterRule &rule) {
                                       return (rule.name == "prefix" && key.starts_with(rule.value)) || (rule.name ==
                                                                                                                 "suffix" &&
                                                                                                         key.ends_with(rule.value));
                                   });
    }

    view_or_value<view, value> TopicNotification::ToDocument() const {
        auto topicNotificationDoc = document{};
        topicNotificationDoc.append(kvp("id", id));
        topicNotificationDoc.append(kvp("topicArn", topicArn));

        // Events
        auto eventsDoc = bsoncxx::builder::basic::array{};
        for (const auto &event: events) {
            eventsDoc.append(event);
        }
        topicNotificationDoc.append(kvp("events", eventsDoc));

        auto filterRulesDoc = bsoncxx::builder::basic::array{};
        for (const auto &filterRule: filterRules) {
            filterRulesDoc.append(filterRule.ToDocument());
        }
        topicNotificationDoc.append(kvp("filterRules", filterRulesDoc));

        return topicNotificationDoc.extract();
    }

    TopicNotification TopicNotification::FromDocument(std::optional<view> mResult) {
        try {
            id = Core::Bson::BsonUtils::GetStringValue(mResult.value()["id"]);
            topicArn = Core::Bson::BsonUtils::GetStringValue(mResult.value()["topicArn"]);

            // Extract filter rules
            if (mResult.value().find("filterRules") != mResult.value().end()) {
                filterRules.clear();
                for (const bsoncxx::array::view filterRulesView{mResult.value()["filterRules"].get_array().value}; const bsoncxx::array::element &filterRuleElement: filterRulesView) {
                    FilterRule filterRule;
                    filterRule.FromDocument(filterRuleElement.get_document());
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
