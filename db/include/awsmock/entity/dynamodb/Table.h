//
// Created by vogje01 on 07/06/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/dynamodb/AttributeDefinition.h>
#include <awsmock/entity/dynamodb/KeySchema.h>
#include <awsmock/entity/dynamodb/ProvisionedThroughput.h>
#include <awsmock/entity/dynamodb/StreamSpecification.h>
#include <awsmock/entity/dynamodb/Tag.h>

namespace Awsmock::Database::Entity::DynamoDb {

    using std::chrono::system_clock;

    /**
     * @brief DynamoDB table entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Table final : Common::BaseEntity<Table> {

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
        std::vector<AttributeDefinition> attributeDefinitions;

        /**
         * Key schemas
         */
        std::vector<KeySchema> keySchema;

        /**
         * Tags
         */
        std::vector<Tag> tags;

        /**
         * Item count
         */
        std::int64_t items{};

        /**
         * Size in bytes
         */
        std::int64_t size{};

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Stream specification
         */
        StreamSpecification streamSpecification;

        /**
         * Billing mode (PAY_PER_REQUEST or PROVISIONED)
         */
        std::string billingMode;

        /**
         * Server-side encryption enabled
         */
        bool sseEnabled{false};

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Get partition key name
         */
        [[nodiscard]] std::string GetPartitionKeyName() const {
            for (const auto &key: keySchema) {
                if (key.keyType == "HASH") {
                    return key.attributeName;
                }
            }
            return {};
        }

        /**
         * @brief Get partition key name
         */
        [[nodiscard]] std::string GetSortKeyName() const {
            for (const auto &key: keySchema) {
                if (key.keyType == "RANGE") {
                    return key.attributeName;
                }
            }
            return {};
        }

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
        void FromDocument(const std::optional<view> &mResult);

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "DynamoDB"};
    };

    typedef std::vector<Table> TableList;

}// namespace Awsmock::Database::Entity::DynamoDb
