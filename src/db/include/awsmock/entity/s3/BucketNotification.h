//
// Created by vogje01 on 06/09/2023.
//

#ifndef AWSMOCK_DB_ENTITY_S3_BUCKET_NOTIFICATION_H
#define AWSMOCK_DB_ENTITY_S3_BUCKET_NOTIFICATION_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief S3 bucket notification entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct BucketNotification final : Common::BaseEntity<BucketNotification> {

        /**
         * Event
         */
        std::string event;

        /**
         * Notification ID
         */
        std::string notificationId;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const override;
    };

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_BUCKET_NOTIFICATION_H
