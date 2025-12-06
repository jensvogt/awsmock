//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/s3/Bucket.h>

namespace AwsMock::Database::Entity::S3 {

    bool Bucket::HasNotification(const std::string &eventName) {
        return std::ranges::find_if(notifications, [eventName](const BucketNotification &eventNotification) {
                   return eventNotification.event == eventName;
               }) != notifications.end();
    }

    bool Bucket::HasQueueNotificationId(const std::string &id) {
        return std::ranges::find_if(queueNotifications, [id](const QueueNotification &notification) {
                   return notification.id == id;
               }) != queueNotifications.end();
    }

    bool Bucket::HasTopicNotificationId(const std::string &id) {
        return std::ranges::find_if(topicNotifications, [id](const TopicNotification &notification) {
                   return notification.id == id;
               }) != topicNotifications.end();
    }

    bool Bucket::HasLambdaNotificationId(const std::string &id) {
        return std::ranges::find_if(lambdaNotifications, [id](const LambdaNotification &notification) {
                   return notification.id == id;
               }) != lambdaNotifications.end();
    }

    bool Bucket::HasQueueNotification(const std::string &queueArn) const {
        return std::ranges::find_if(queueNotifications, [queueArn](const QueueNotification &notification) {
                   return notification.queueArn == queueArn;
               }) != queueNotifications.end();
    }

    bool Bucket::HasTopicNotification(const std::string &topicArn) const {
        return std::ranges::find_if(topicNotifications, [topicArn](const TopicNotification &notification) {
                   return notification.topicArn == topicArn;
               }) != topicNotifications.end();
    }

    bool Bucket::HasLambdaNotification(const std::string &lambdaArn) const {
        return std::ranges::find_if(lambdaNotifications, [lambdaArn](const LambdaNotification &notification) {
                   return notification.lambdaArn == lambdaArn;
               }) != lambdaNotifications.end();
    }

    bool Bucket::HasLambdaNotificationEvent(const std::string &event) const {
        return std::ranges::find_if(lambdaNotifications, [event](const LambdaNotification &notification) {
                   return std::ranges::find(notification.events, event) != notification.events.end();
               }) != lambdaNotifications.end();
    }

    bool Bucket::HasQueueNotificationEvent(const std::string &event) const {
        return std::ranges::find_if(queueNotifications, [event](const QueueNotification &notification) {
                   return std::ranges::find(notification.events, event) != notification.events.end();
               }) != queueNotifications.end();
    }

    bool Bucket::HasTopicNotificationEvent(const std::string &event) const {
        return std::ranges::find_if(topicNotifications, [event](const TopicNotification &notification) {
                   return std::ranges::find(notification.events, event) != notification.events.end();
               }) != topicNotifications.end();
    }

    bool Bucket::HasEncryption() const {
        return !bucketEncryption.kmsKeyId.empty() && !bucketEncryption.sseAlgorithm.empty();
    }

    BucketNotification Bucket::GetNotification(const std::string &eventName) {
        const auto it =
                std::ranges::find_if(notifications, [eventName](const BucketNotification &eventNotification) {
                    return eventNotification.event == eventName;
                });
        return *it;
    }

    QueueNotification Bucket::GetQueueNotification(const std::string &eventName) {
        return *std::ranges::find_if(queueNotifications, [eventName](const QueueNotification &eventNotification) {
            return std::ranges::find(eventNotification.events, eventName) != eventNotification.events.end();
        });
    }

    TopicNotification Bucket::GetTopicNotification(const std::string &eventName) {
        return *std::ranges::find_if(topicNotifications, [eventName](const TopicNotification &eventNotification) {
            return std::ranges::find(eventNotification.events, eventName) != eventNotification.events.end();
        });
    }

    LambdaNotification Bucket::GetLambdaNotification(const std::string &eventName) {
        return *std::ranges::find_if(lambdaNotifications, [eventName](const LambdaNotification &eventNotification) {
            return std::ranges::find(eventNotification.events, eventName) != eventNotification.events.end();
        });
    }

    LambdaNotification Bucket::GetLambdaNotificationByArn(const std::string &arn) {
        const auto it = std::ranges::find_if(lambdaNotifications, [arn](const LambdaNotification &eventNotification) {
            return eventNotification.lambdaArn == arn;
        });
        if (it != lambdaNotifications.end()) {
            return *it;
        }
        return {};
    }

    bool Bucket::IsVersioned() const {
        return versionStatus == ENABLED;
    }

    view_or_value<view, value> Bucket::ToDocument() const {

        // Bucket notifications are deprecated, should be removed at a certain point
        auto notificationsDoc = array{};
        for (const auto &notification: notifications) {
            notificationsDoc.append(notification.ToDocument());
        }

        // Queue notifications
        auto queueNotificationsDoc = array{};
        for (const auto &notification: queueNotifications) {
            queueNotificationsDoc.append(notification.ToDocument());
        }

        // Topic notifications
        auto topicNotificationsDoc = array{};
        for (const auto &notification: topicNotifications) {
            topicNotificationsDoc.append(notification.ToDocument());
        }

        // Lambda notifications
        auto lambdaNotificationsDoc = array{};
        for (const auto &notification: lambdaNotifications) {
            lambdaNotificationsDoc.append(notification.ToDocument());
        }

        view_or_value<view, value> bucketDoc = make_document(
                kvp("region", region),
                kvp("name", name),
                kvp("owner", owner),
                kvp("arn", arn),
                kvp("size", bsoncxx::types::b_int64(size)),
                kvp("keys", bsoncxx::types::b_int64(keys)),
                kvp("notifications", notificationsDoc),
                kvp("queueNotifications", queueNotificationsDoc),
                kvp("topicNotifications", topicNotificationsDoc),
                kvp("lambdaConfigurations", lambdaNotificationsDoc),
                kvp("encryptionConfiguration", bucketEncryption.ToDocument()),
                kvp("versionStatus", BucketVersionStatusToString(versionStatus)),
                kvp("created", bsoncxx::types::b_date(created)),
                kvp("modified", bsoncxx::types::b_date(modified)));

        return bucketDoc;
    }

    void Bucket::FromDocument(std::optional<view> mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        owner = Core::Bson::BsonUtils::GetStringValue(mResult, "owner");
        arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
        size = Core::Bson::BsonUtils::GetLongValue(mResult.value()["size"]);
        keys = Core::Bson::BsonUtils::GetLongValue(mResult.value()["keys"]);
        versionStatus = BucketVersionStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult, "versionStatus"));
        created = Core::Bson::BsonUtils::GetDateValue(mResult.value()["created"]);
        modified = Core::Bson::BsonUtils::GetDateValue(mResult.value()["modified"]);

        // SQS queue notification configuration
        if (mResult.value().find("queueNotifications") != mResult.value().end()) {
            queueNotifications.clear();
            for (bsoncxx::array::view notificationView{mResult.value()["queueNotifications"].get_array().value}; const bsoncxx::array::element &notificationElement: notificationView) {
                QueueNotification notification;
                queueNotifications.emplace_back(notification.FromDocument(notificationElement.get_document().view()));
            }
        }

        // SNS topic notification configuration
        if (mResult.value().find("topicNotifications") != mResult.value().end()) {
            topicNotifications.clear();
            for (bsoncxx::array::view notificationView{mResult.value()["topicNotifications"].get_array().value}; const bsoncxx::array::element &notificationElement: notificationView) {
                TopicNotification notification;
                topicNotifications.emplace_back(notification.FromDocument(notificationElement.get_document().view()));
            }
        }

        // Lambda function notification configuration
        if (mResult.value().find("lambdaConfigurations") != mResult.value().end()) {
            lambdaNotifications.clear();
            for (bsoncxx::array::view notificationView{mResult.value()["lambdaConfigurations"].get_array().value}; const bsoncxx::array::element &notificationElement: notificationView) {
                LambdaNotification notification;
                lambdaNotifications.emplace_back(notification.FromDocument(notificationElement.get_document().view()));
            }
        }
    }

}// namespace AwsMock::Database::Entity::S3