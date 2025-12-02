//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_ENTITY_DYNAMODB_PROVISIONED_THROUGHPUT_H
#define AWSMOCK_ENTITY_DYNAMODB_PROVISIONED_THROUGHPUT_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace AwsMock::Database::Entity::DynamoDb {

    /**
     * @brief DynamoDB provisioned throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ProvisionedThroughput final : Common::BaseEntity<ProvisionedThroughput> {

        /**
         * Read capacity units
         */
        long readCapacityUnits{};

        /**
         * Write capacity units
         */
        long writeCapacityUnits{};

        /**
         * Last decrease time
         */
        system_clock::time_point lastDecreaseDateTime;

        /**
         * Last increase time
         */
        system_clock::time_point lastIncreaseDateTime;

        /**
         * Number of decreases
         */
        long numberOfDecreasesToday{};

        /**
         * @brief Convert to a BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Convert from a BSON document
         */
        void FromDocument(const std::optional<view> &document);
    };

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_ENTITY_DYNAMODB_PROVISIONED_THROUGHPUT_H
