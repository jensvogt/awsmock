//
// Created by vogje01 on 06/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_TOPIC_NOTIFICATION_H
#define AWSMOCK_DB_ENTITY_S3_TOPIC_NOTIFICATION_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/s3/FilterRule.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief S3 bucket topic notification entity
     *
     * @par
     * This is a child object of the bucket entity.
     *
     * @see AwsMock::Database::Entity::Bucket
     * @author jens.vogt\@opitz-consulting.com
     */
    struct TopicNotification final : Common::BaseEntity<TopicNotification> {

        /**
         * ID
         */
        std::string id;

        /**
         * Event
         */
        std::vector<std::string> events;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Filter rules
         */
        std::vector<FilterRule> filterRules;

        /**
         * @brief Check filter
         *
         * @param key object key
         * @return true in case filter exists and key matches
         */
        bool CheckFilter(const std::string &key);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         * @return topic notification entity
         */
        static TopicNotification FromDocument(const std::optional<view> &mResult);
    };

} // namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_TOPIC_NOTIFICATION_H
