//
// Created by vogje01 on 07/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_DYNAMODB_TABLE_H
#define AWSMOCK_DB_ENTITY_DYNAMODB_TABLE_H

// C++ includes
#include <chrono>
#include <map>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/dynamodb/ProvisionedThroughput.h>
#include <awsmock/entity/dynamodb/StreamSpecification.h>

namespace AwsMock::Database::Entity::DynamoDb {

    using std::chrono::system_clock;

    /**
     * @brief DynamoDB table entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Table {

        /**
         * ID
         */
        std::string oid;

        /**
         * AWS region name
         */
        std::string region;

        /**
         * Name
         */
        std::string name;

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Table status
         */
        std::string status;

        /**
         * Attributes
         */
        std::vector<std::map<std::string, std::string>> attributes;

        /**
         * Key schemas
         */
        std::vector<std::map<std::string, std::string>> keySchemas;

        /**
         * Tags
         */
        std::vector<std::map<std::string, std::string>> tags;

        /**
         * Item count
         */
        long itemCount{};

        /**
         * Size in bytes
         */
        long size{};

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Stream specification
         */
        StreamSpecification streamSpecification;

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
        void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the entity to a JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const Table &d);
    };

    typedef std::vector<Table> TableList;

}// namespace AwsMock::Database::Entity::DynamoDb

#endif// AWSMOCK_DB_ENTITY_DYNAMODB_TABLE_H
