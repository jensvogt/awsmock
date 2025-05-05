//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_DYNAMODB_ITEM_H
#define AWSMOCK_DB_ENTITY_DYNAMODB_ITEM_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/dynamodb/AttributeValue.h>
#include <awsmock/entity/dynamodb/Table.h>

namespace AwsMock::Database::Entity::DynamoDb {

    using std::optional;
    using std::chrono::system_clock;

    /**
     * @brief DynamoDB item primary key
     * @code(.json)
     * {
     *   "featureCustom": {
     *     "N": "1024"
     *   },
     *   "featureName": {
     *     "S": "ONIX_PARSING"
     *   },
     *   "featureState": {
     *     "S": "{\"enabled\":true,\"strategyId\":null,\"parameters\":{}}"
     *   }
     * }
     * @endcode
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Item {

        /**
         * ID
         */
        std::string oid;

        /**
         * AWS region name
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Item size in bytes
         */
        long size{};

        /**
         * Attributes
         */
        std::map<std::string, AttributeValue> attributes;

        /**
         * Keys
         */
        std::map<std::string, AttributeValue> keys;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

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
        Item FromDocument(const view_or_value<view, value> &mResult);

        /**
         * @brief Converts the DynamoDB document to a MongoDb entity
         *
         * @param mResult query result.
         */
        Item FromDynamodb(const view_or_value<view, value> &mResult);

        /**
         * Converts the entity to a JSON string
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToJson() const;

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
         * @param d DynamoDB  entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Item &d);
    };

    typedef std::vector<Item> ItemList;

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_DB_ENTITY_DYNAMODB_ITEM_H
