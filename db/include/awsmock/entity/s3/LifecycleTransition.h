//
// Created by vogje01 on 1/9/26.
//

#ifndef AWSMOCK_DB_ENTITY_S3_LIFECYCLE_TRANSITION_H
#define AWSMOCK_DB_ENTITY_S3_LIFECYCLE_TRANSITION_H

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/s3/StorageClass.h>

namespace AwsMock::Database::Entity::S3 {

    /**
     * @brief Filter rule for the S3 bucket notification to SQS queues
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LifecycleTransition final : Common::BaseEntity<LifecycleTransition> {

        /**
         * @brief Timestamp
         */
        system_clock::time_point date;

        /**
         * @brief Number of days
         */
        int days{};

        /**
         * @brief Storage class
         */
        StorageClass storeClass;

        /**
         * @brief Default constructor
         */
        LifecycleTransition() = default;

        /**
         * @brief Constructor
         *
         * @param mResult BSON view
         */
        explicit LifecycleTransition(const view &mResult);

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
        [[maybe_unused]] void FromDocument(const view &mResult);
    };

}// namespace AwsMock::Database::Entity::S3

#endif// AWSMOCK_DB_ENTITY_S3_LIFECYCLE_TRANSITION_H
