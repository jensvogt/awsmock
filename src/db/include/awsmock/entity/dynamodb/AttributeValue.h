//
// Created by vogje01 on 12/21/23.
//

#ifndef AWSMOCK_ENTITY_DYNAMODB_ATTRIBUTE_VALUE_H
#define AWSMOCK_ENTITY_DYNAMODB_ATTRIBUTE_VALUE_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock include
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::DynamoDb {

    /**
     * @brief DynamoDB attribute value entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AttributeValue {

        /**
         * String value
         */
        std::string stringValue;

        /**
         * String set value
         */
        std::vector<std::string> stringSetValue;

        /**
         * Number value
         */
        std::string numberValue;

        /**
         * Number set value
         */
        std::vector<std::string> numberSetValue;

        /**
         * Boolean value
         */
        std::shared_ptr<bool> boolValue;

        /**
         * Null value
         */
        std::shared_ptr<bool> nullValue;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(view_or_value<view, value> mResult);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const AttributeValue &r);
    };
}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_ENTITY_DYNAMODB_ATTRIBUTE_VALUE_H
