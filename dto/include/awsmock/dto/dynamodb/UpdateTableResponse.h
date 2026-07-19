//
// Created by vogje01 on 19/07/2026.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeDefinition.h>
#include <awsmock/dto/dynamodb/model/KeySchema.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>
#include <awsmock/dto/dynamodb/model/Tag.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief DynamoDB update table response
     *
     * The response wraps the table description under the "TableDescription" key.
     */
    struct UpdateTableResponse final : Common::BaseCounter<UpdateTableResponse> {

        /**
         * @brief Name of the table
         */
        std::string tableName;

        /**
         * @brief AWS ARN
         */
        std::string tableArn;

        /**
         * @brief Billing mode (PAY_PER_REQUEST or PROVISIONED)
         */
        std::string billingMode;

        /**
         * @brief Total size of table in bytes
         */
        long tableSize{};

        /**
         * @brief Total number of items
         */
        long items{};

        /**
         * @brief Status of the table
         */
        TableStatusType tableStatus = TableStatusType::ACTIVE;

        /**
         * @brief Creation date
         */
        system_clock::time_point createdDateTime = system_clock::now();

        /**
         * @brief Deletion protection
         */
        bool deletionProtectionEnabled{};

        /**
         * @brief Server-side encryption
         */
        bool sseEnabled{false};

        /**
         * @brief Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput{};

        /**
         * @brief Table key schema
         */
        std::vector<KeySchema> keySchema{};

        /**
         * @brief Table attributes
         */
        std::vector<AttributeDefinition> attributeDefinitions{};

        /**
         * @brief Table tags
         */
        std::vector<Tag> tags{};

      private:

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateTableResponse const &obj) {
            boost::json::object tableObject;
            tableObject["TableName"] = obj.tableName;
            tableObject["TableArn"] = obj.tableArn;
            tableObject["TableSizeBytes"] = obj.tableSize;
            tableObject["ItemCount"] = obj.items;
            tableObject["TableStatus"] = TableStatusTypeToString(obj.tableStatus);
            tableObject["CreationDateTime"] = Core::DateTimeUtils::UnixTimestamp(obj.createdDateTime);
            tableObject["DeletionProtectionEnabled"] = obj.deletionProtectionEnabled;
            tableObject["ProvisionedThroughput"] = boost::json::value_from(obj.provisionedThroughput);
            if (!obj.billingMode.empty()) {
                tableObject["BillingModeSummary"] = boost::json::object{{"BillingMode", obj.billingMode}};
            }
            if (obj.sseEnabled) {
                tableObject["SSEDescription"] = boost::json::object{{"Status", "ENABLED"}, {"SSEType", "KMS"}};
            }
            if (!obj.tags.empty()) {
                tableObject["Tags"] = boost::json::value_from(obj.tags);
            }
            if (!obj.keySchema.empty()) {
                tableObject["KeySchema"] = boost::json::value_from(obj.keySchema);
            }
            if (!obj.attributeDefinitions.empty()) {
                tableObject["AttributeDefinitions"] = boost::json::value_from(obj.attributeDefinitions);
            }
            boost::json::object root;
            root["TableDescription"] = std::move(tableObject);
            jv = std::move(root);
        }
    };

}// namespace Awsmock::Dto::DynamoDb
