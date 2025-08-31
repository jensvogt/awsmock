//
// Created by vogje01 on 01/06/2023.
//

#include <awsmock/entity/sns/Topic.h>

namespace AwsMock::Database::Entity::SNS {

    bool Topic::HasSubscription(const Subscription &subscription) {
        return std::ranges::find_if(subscriptions, [subscription](const Subscription &s) {
                   return s.protocol == subscription.protocol && s.endpoint == subscription.endpoint;
               }) != subscriptions.end();
    }

    bool Topic::HasSubscription(const std::string &subscriptionArn) {
        return std::ranges::find_if(subscriptions, [subscriptionArn](const Subscription &s) {
                   return s.subscriptionArn == subscriptionArn;
               }) != subscriptions.end();
    }

    int Topic::GetSubscriptionIndex(const std::string &subscriptionArn) {
        const auto it = std::ranges::find_if(subscriptions, [subscriptionArn](const Subscription &s) {
            return s.subscriptionArn == subscriptionArn;
        });
        if (it != subscriptions.end()) {
            return static_cast<int>(std::distance(subscriptions.begin(), it));
        }
        return -1;
    }

    view_or_value<view, value> Topic::ToDocument() const {

        auto subscriptionDocs = array{};
        if (!subscriptions.empty()) {
            for (const auto &subscription: subscriptions) {
                subscriptionDocs.append(subscription.ToDocument());
            }
        }

        auto tagsDoc = document{};
        if (!tags.empty()) {
            for (const auto &[fst, snd]: tags) {
                tagsDoc.append(kvp(fst, snd));
            }
        }

        document topicDoc;
        Core::Bson::BsonUtils::SetStringValue(topicDoc, "region", region);
        Core::Bson::BsonUtils::SetStringValue(topicDoc, "topicName", topicName);
        Core::Bson::BsonUtils::SetStringValue(topicDoc, "topicArn", topicArn);
        Core::Bson::BsonUtils::SetStringValue(topicDoc, "targetArn", targetArn);
        Core::Bson::BsonUtils::SetLongValue(topicDoc, "messages", messages);
        Core::Bson::BsonUtils::SetLongValue(topicDoc, "messagesSend", messagesSend);
        Core::Bson::BsonUtils::SetLongValue(topicDoc, "messagesResend", messagesResend);
        Core::Bson::BsonUtils::SetLongValue(topicDoc, "size", size);
        Core::Bson::BsonUtils::SetDateValue(topicDoc, "created", created);
        Core::Bson::BsonUtils::SetDateValue(topicDoc, "modified", modified);

        topicDoc.append(kvp("subscriptions", subscriptionDocs));
        topicDoc.append(kvp("attributes", topicAttribute.ToDocument()));
        topicDoc.append(kvp("tags", tagsDoc));
        return topicDoc.extract();
    }

    void Topic::FromDocument(const view_or_value<view, value> &mResult) {

        try {

            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            topicName = Core::Bson::BsonUtils::GetStringValue(mResult, "topicName");
            owner = Core::Bson::BsonUtils::GetStringValue(mResult, "owner");
            topicArn = Core::Bson::BsonUtils::GetStringValue(mResult, "topicArn");
            targetArn = Core::Bson::BsonUtils::GetStringValue(mResult, "targetArn");
            size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
            messages = Core::Bson::BsonUtils::GetLongValue(mResult, "messages");
            messagesSend = Core::Bson::BsonUtils::GetLongValue(mResult, "messagesSend");
            messagesResend = Core::Bson::BsonUtils::GetLongValue(mResult, "messagesResend");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Attributes
            if (mResult.view().find("attributes") != mResult.view().end()) {
                topicAttribute.FromDocument(mResult.view()["attributes"].get_document().view());
            }

            // Subscriptions
            if (mResult.view().find("subscriptions") != mResult.view().end()) {
                subscriptions.clear();
                for (const bsoncxx::array::view subscriptionsView{mResult.view()["subscriptions"].get_array().value}; const bsoncxx::array::element &subscriptionElement: subscriptionsView) {
                    Subscription subscription{
                            .protocol = bsoncxx::string::to_string(subscriptionElement["protocol"].get_string().value),
                            .endpoint = bsoncxx::string::to_string(subscriptionElement["endpoint"].get_string().value),
                            .subscriptionArn = bsoncxx::string::to_string(subscriptionElement["subscriptionArn"].get_string().value)};
                    subscriptions.emplace_back(subscription);
                }
            }

            // Get tags
            if (mResult.view().find("tags") != mResult.view().end()) {
                for (const view tagsView = mResult.view()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                    std::string key = bsoncxx::string::to_string(tagElement.key());
                    std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                    tags.emplace(key, value);
                }
            }

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Topic::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Topic::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Topic &t) {
        os << "Topic=" << to_json(t.ToDocument());
        return os;
    }

}// namespace AwsMock::Database::Entity::SNS
