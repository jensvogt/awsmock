//
// Created by vogje01 on 03/09/2023.
//

#pragma once

// C++ includes
#include <string>

// MongoDB includes
#include <awsmock/core/BsonUtils.h>

namespace Awsmock::Database::Entity::Lambda {

    /**
     * @brief Lambda event source mapping
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EventSourceMapping {

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

        /**
         * Event source type; can be one of SQS, SNS, S3
         */
        std::string type;

        /**
         * Batch size
         */
        long batchSize = 10;

        /**
         * Maximum BatchingWindowInSeconds
         */
        long maximumBatchingWindowInSeconds = 5;

        /**
         * Enabled
         */
        bool enabled = false;

        /**
         * UUID
         */
        std::string uuid;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]] void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param e event source mapping
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const EventSourceMapping &e);
    };

}// namespace Awsmock::Database::Entity::Lambda
