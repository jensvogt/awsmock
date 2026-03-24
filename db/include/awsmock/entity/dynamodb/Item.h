//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_DYNAMODB_ITEM_H
#define AWSMOCK_DB_ENTITY_DYNAMODB_ITEM_H

// C++ includes
#include <set>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/dynamodb/AttributeValue.h>

namespace AwsMock::Database::Entity::DynamoDb {

    // Forward declare for nested types
    struct DynamoValue;

    using KeyValue = std::variant<std::string, double, std::vector<uint8_t>>;
    using DynamoList = std::vector<std::shared_ptr<DynamoValue>>;
    using DynamoMap = std::unordered_map<std::string, std::shared_ptr<DynamoValue>>;

    // Wrapper to distinguish Binary from Binary Set
    struct BinarySet {
        std::vector<std::vector<uint8_t>> values;
    };

    struct DynamoValue {
        using DynamoVariant = std::variant<
                std::string,          // S
                double,               // N
                std::vector<uint8_t>, // B
                bool,                 // BOOL
                std::nullptr_t,       // NULL
                std::set<std::string>,// SS
                std::set<double>,     // NS
                BinarySet,            // BS
                DynamoList,           // L
                DynamoMap             // M
                >;

        DynamoVariant value;
    };

    // Forward declaration
    bsoncxx::types::bson_value::value DynamoValueToBson(const DynamoValue &dv);

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
    struct Item final : Common::BaseEntity<Item> {

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
         * Partition key
         */
        KeyValue partitionKey;

        /**
         * Sortkey key
         */
        KeyValue sortKey;

        /**
         * Item size in bytes
         */
        long size{};

        /**
         * Attributes
         */
        std::unordered_map<std::string, DynamoValue> attributes;

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const override;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        Item FromDocument(const view_or_value<view, value> &mResult);

        /**
         * @brief Convert keys to BSON
         *
         * @param kv key
         * @return BSON object
         */
        static bsoncxx::types::bson_value::value KeyValueToBson(const KeyValue &kv);

        /**
         * @brief Convert keys from BSON
         *
         * @param val BSON object
         * @return value
         */
        static KeyValue KeyValueFromBson(const bsoncxx::types::bson_value::view &val);

        bsoncxx::types::bson_value::value DynamoValueToBson(const DynamoValue &dv) const;

        static DynamoValue DynamoValueFromBson(const bsoncxx::types::bson_value::view &val);

        static std::unordered_map<std::string, DynamoValue> AttributesFromBson(const view &doc);

        /**
         * @brief Equality operator
         */
        bool operator==(const Item &other) const {
            return partitionKey == other.partitionKey && sortKey == other.sortKey;
        }
    };

    typedef std::vector<Item> ItemList;

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_DB_ENTITY_DYNAMODB_ITEM_H
