//
// Created by vogje01 on 4/8/24.
//

#ifndef AWSMOCK_DB_ENTITY_SECRETSMANAGER_ROTATION_RULES_H
#define AWSMOCK_DB_ENTITY_SECRETSMANAGER_ROTATION_RULES_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Database::Entity::SecretsManager {

    /**
     * Rotation rules
     *
     * Example:
     * @code{.json}
     * {
     *   "AutomaticallyAfterDays": number,
     *   "Duration": "string",
     *   "ScheduleExpression": "string"
     * }
     * @endcode
     */
    struct RotationRules {

        /**
         * Automatic rotation period
         */
        long automaticallyAfterDays{};

        /**
         * Duration
         */
        std::string duration;

        /**
         * Duration
         */
        std::string scheduleExpression;

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        void FromDocument(const std::optional<view> &mResult);

        /**
         * Converts the entity to a string representation.
         *
         * @return entity as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const RotationRules &r);
    };

}// namespace AwsMock::Database::Entity::SecretsManager

#endif// AWSMOCK_ENTITY_SECRETSMANAGER_ROTATION_RULES_H
