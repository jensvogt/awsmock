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

namespace AwsMock::Database::Entity::DynamoDb {

    using std::chrono::system_clock;

    /**
     * @brief DynamoDB provisioned throughput
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ProvisionedThroughput {

        /**
         * Read capacity units
         */
        long readCapacityUnits = 0;

        /**
         * Write capacity units
         */
        long writeCapacityUnits = 0;

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
        long numberOfDecreasesToday = 0;

        /**
         * @brief Convert to a BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert from a BSON document
         */
        void FromDocument(const std::optional<view> &document);
        /**
         * @brief Converts the entity to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] std::string ToJson() const;

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
        friend std::ostream &operator<<(std::ostream &os, const ProvisionedThroughput &r);
    };

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_ENTITY_DYNAMODB_PROVISIONED_THROUGHPUT_H
