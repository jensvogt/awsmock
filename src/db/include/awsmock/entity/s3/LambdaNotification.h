//
// Created by vogje01 on 06/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_LAMBDA_NOTIFICATION_H
#define AWSMOCK_DB_ENTITY_S3_LAMBDA_NOTIFICATION_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/s3/FilterRule.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief S3 bucket lambda notification entity.
     *
     * @par
     * This is a child object of the bucket entity.
     *
     * @see AwsMock::Database::Entity::Bucket
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LambdaNotification final : Common::BaseEntity<LambdaNotification> {

        /**
         * ID
         */
        std::string id;

        /**
         * Event
         */
        std::vector<std::string> events;

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

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
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        LambdaNotification FromDocument(const std::optional<view> &mResult);
    };

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_LAMBDA_NOTIFICATION_H
