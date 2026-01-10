//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_BUCKET_H
#define AWSMOCK_DB_ENTITY_S3_BUCKET_H
#include "LifecycleConfiguration.h"

#ifdef _WIN32
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMock include
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/s3/BucketEncryption.h>
#include <awsmock/entity/s3/BucketNotification.h>
#include <awsmock/entity/s3/LambdaNotification.h>
#include <awsmock/entity/s3/QueueNotification.h>
#include <awsmock/entity/s3/TopicNotification.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief Versioning status
     */
    enum BucketVersionStatus {
        ENABLED,
        SUSPENDED,
        DISABLED
    };

    static std::map<BucketVersionStatus, std::string> BucketVersionStatusNames{
            {ENABLED, "enabled"},
            {SUSPENDED, "suspended"},
            {DISABLED, "disabled"},
    };

    [[maybe_unused]] static std::string BucketVersionStatusToString(const BucketVersionStatus bucketVersionStatus) {
        return BucketVersionStatusNames[bucketVersionStatus];
    }

    [[maybe_unused]] static BucketVersionStatus BucketVersionStatusFromString(const std::string &bucketVersionStatus) {
        for (auto &[fst, snd]: BucketVersionStatusNames) {
            if (Core::StringUtils::EqualsIgnoreCase(snd, bucketVersionStatus)) {
                return fst;
            }
        }
        return DISABLED;
    }

    /**
     * @brief S3 bucket entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Bucket final : Common::BaseEntity<Bucket> {

        /**
         * ID
         */
        std::string oid;

        /**
         * Bucket region
         */
        std::string region;

        /**
         * Bucket name
         */
        std::string name;

        /**
         * Bucket owner
         */
        std::string owner;

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Size in bytes
         */
        long size{};

        /**
         * Number of object keys
         */
        long keys{};

        /**
         * Bucket notifications
         */
        std::vector<BucketNotification> notifications;

        /**
         * Queue notification configurations
         */
        std::vector<QueueNotification> queueNotifications;

        /**
         * Topic notification configurations
         */
        std::vector<TopicNotification> topicNotifications;

        /**
         * Lambda notification configurations
         */
        std::vector<LambdaNotification> lambdaNotifications;

        /**
         * Default metadata
         */
        std::map<std::string, std::string> defaultMetadata;

        /**
         * S3 lifecycle configurations
         */
        std::vector<LifecycleConfiguration> lifecycleConfigurations;

        /**
         * Bucket versioning state
         */
        BucketVersionStatus versionStatus = DISABLED;

        /**
         * Bucket encryption configurations
         */
        BucketEncryption bucketEncryption;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Checks whether a notification with the given event name exists.
         *
         * @param eventName name of the event
         * @return true if notification with the given event name exists.
         * @deprecated use HasQueueNotification,HasTopicNotification, HasLambdaNotification
         */
        bool HasNotification(const std::string &eventName);

        /**
         * @brief Checks whether a notification with the given ID exists.
         *
         * @param id other queue notification id.
         * @return true if notification with the given ID exists.
         */
        bool HasQueueNotificationId(const std::string &id);

        /**
         * @brief Checks whether a topic notification with the given ID exists.
         *
         * @param id another topic notification id.
         * @return true if notification with the given ID exists.
         */
        bool HasTopicNotificationId(const std::string &id);

        /**
         * @brief Checks whether a lambda notification with the given ID exists.
         *
         * @param id other lambda notification ID.
         * @return true if notification with the given ID exists.
         */
        bool HasLambdaNotificationId(const std::string &id);

        /**
         * @brief Checks whether a SQS queue notification with the given event name exists.
         *
         * @param queueArn AWS queue ARN
         * @return true if notification with the given event name exists.
         */
        [[nodiscard]] bool HasQueueNotification(const std::string &queueArn) const;

        /**
         * @brief Checks whether a SNS topic notification with the given event name exists.
         *
         * @param topicArn AWS topic Arn
         * @return true if notification with the given event name exists.
         */
        [[nodiscard]] bool HasTopicNotification(const std::string &topicArn) const;

        /**
         * @brief Checks whether a lambda notification with the given event name exists.
         *
         * @param lambdaArn AWS Lambda ARN
         * @return true if notification with the given event name exists.
         */
        [[nodiscard]] bool HasLambdaNotification(const std::string &lambdaArn) const;

        /**
         * @brief Checks whether a SQS queue notification with the given event name exists.
         *
         * @param event AWS event
         * @return true if notification with the given event name exists.
         */
        [[nodiscard]] bool HasQueueNotificationEvent(const std::string &event) const;

        /**
         * @brief Checks whether a SNS topic notification with the given event name exists.
         *
         * @param event AWS event
         * @return true if notification with the given event name exists.
         */
        [[nodiscard]] bool HasTopicNotificationEvent(const std::string &event) const;

        /**
         * @brief Checks whether a lambda notification with the given event name exists.
         *
         * @param event AWS event
         * @return true if notification with the given event name exists.
         */
        [[nodiscard]] bool HasLambdaNotificationEvent(const std::string &event) const;

        /**
         * @brief Checks whether a bucket has encryption enabled.
         *
         * @return true if encryption is enabled for the bucket.
         */
        [[nodiscard]] bool HasEncryption() const;

        /**
         * @brief Returns a given notification by name
         *
         * @param eventName name of the event
         * @return found notification or notifications.end().
         * @deprecated use GetQueueNotification,GetTopicNotification, GetLambdaNotification
         */
        BucketNotification GetNotification(const std::string &eventName);

        /**
         * @brief Returns a given SQS queue notification by name
         *
         * @param eventName name of the event
         * @return found notification or notifications.end().
         */
        QueueNotification GetQueueNotification(const std::string &eventName);

        /**
         * @brief Returns a given SNS queue notification by name
         *
         * @param eventName name of the event
         * @return found notification or notifications.end().
         */
        TopicNotification GetTopicNotification(const std::string &eventName);

        /**
         * @brief Returns a given lambda notification by name
         *
         * @param eventName name of the event
         * @return found notification or notifications.end().
         */
        LambdaNotification GetLambdaNotification(const std::string &eventName);

        /**
         * @brief Returns a given lambda notification by function arn
         *
         * @param arn function ARN
         * @return found notification or notifications.end().
         */
        LambdaNotification GetLambdaNotificationByArn(const std::string &arn);

        /**
         * @brief Returns a boolean indicating the versioning state
         */
        [[nodiscard]] bool IsVersioned() const;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        [[maybe_unused]] void FromDocument(std::optional<view> mResult);
    };

    typedef std::vector<Bucket> BucketList;

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_BUCKET_H
