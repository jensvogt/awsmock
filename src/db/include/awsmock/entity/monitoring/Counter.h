//
// Created by vogje01 on 10/2/24.
//

#ifndef AWSMOCK_DB_ENTITY_COUNTER_H
#define AWSMOCK_DB_ENTITY_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Database::Entity::Monitoring {

    struct Counter {

        /**
         * Object ID
         */
        std::string oid;

        /**
         * Name
         */
        std::string name;

        /**
         * Label name
         */
        std::string labelName;

        /**
         * Label value
         */
        std::string labelValue;

        /**
         * Value
         */
        double performanceValue;

        /**
         * Timestamp
         */
        system_clock::time_point timestamp = system_clock::now();

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
         * @param counter counter entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Counter &counter);
    };

};// namespace AwsMock::Database::Entity::Monitoring

#endif//AWSMOCK_DB_ENTITY_COUNTER_H
