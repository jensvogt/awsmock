//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>

namespace Awsmock::Database::Entity::DynamoDb {

    /**
     * @brief DynamoDB provisioned throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ProvisionedThroughput final : Common::BaseEntity<ProvisionedThroughput> {

        /**
         * Read capacity units
         */
        std::int64_t readCapacityUnits{};

        /**
         * Write capacity units
         */
        std::int64_t writeCapacityUnits{};

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
        std::int64_t numberOfDecreasesToday{};

        /**
         * @brief Convert to a BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Convert from a BSON document
         */
        void FromDocument(const std::optional<view> &document);
    };

}// namespace Awsmock::Database::Entity::DynamoDb
