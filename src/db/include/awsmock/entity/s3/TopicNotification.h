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
     * S3 bucket topic notification entity
     *
     * <p>
     * This is a child object of the bucket entity.
     * </p>
     *
     * @see AwsMock::Database::Entity::Bucket
     * @author jens.vogt\@opitz-consulting.com
     */
    struct TopicNotification {

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
         * Check filter
         *
         * @param key object key
         * @return true in case filter exists and key matches
         */
        bool CheckFilter(const std::string &key);

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        TopicNotification FromDocument(std::optional<bsoncxx::document::view> mResult);

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const TopicNotification &q);
    };

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_TOPIC_NOTIFICATION_H
