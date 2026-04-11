//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeDefinition.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>
#include <awsmock/dto/dynamodb/model/KeySchema.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>
#include <awsmock/dto/dynamodb/model/TableClassSummary.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>
#include <awsmock/dto/dynamodb/model/Tag.h>
#include <awsmock/entity/dynamodb/AttributeDefinition.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB describe table response
     * @code(json)
     * {
     *   "Table" : {
     *     "AttributeDefinitions" : [ {
     *       "AttributeName" : "Artist",
     *       "AttributeType" : "S"
     *     }, {
     *       "AttributeName" : "SongTitle",
     *       "AttributeType" : "S"
     *     } ],
     *     "TableClassSummary": {
     *       "LastUpdateDateTime": number,
     *       "TableClass": "string"
     *      },
     *     "TableName" : "MusicCollection",
     *     "KeySchema" : [ {
     *       "AttributeName" : "Artist",
     *       "KeyType" : "HASH"
     *       }, {
     *       "AttributeName" : "SongTitle",
     *       "KeyType" : "RANGE"
     *       } ],
     *     "TableStatus" : "ACTIVE",
     *     "CreationDateTime" : 1.747564985299E9,
     *       "ProvisionedThroughput" : {
     *       "LastIncreaseDateTime" : 0.0,
     *       "LastDecreaseDateTime" : 0.0,
     *       "NumberOfDecreasesToday" : 0,
     *       "ReadCapacityUnits" : 5,
     *       "WriteCapacityUnits" : 5
     *     },
     *     "TableSizeBytes" : 0,
     *     "ItemCount" : 0,
     *     "TableArn" : "arn:aws:dynamodb:ddblocal:000000000000:table/MusicCollection",
     *     "DeletionProtectionEnabled" : false
     *   }
     * }
     * @endcode
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeTableResponse final : Common::BaseCounter<DescribeTableResponse> {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Table ARN
         */
        std::string tableArn;

        /**
         * Table size
         */
        long size{};

        /**
         * Item count
         */
        long items{};

        /**
         * Key schema
         */
        std::vector<KeySchema> keySchema{};

        /**
         * Attribute definitions
         */
        std::vector<AttributeDefinition> attributeDefinitions{};

        /**
         * Tags
         */
        std::vector<Tag> tags{};

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput{};

        /**
         * Table class summary
         */
        TableClassSummary tableClassSummary{};

        /**
         * Table status
         */
        TableStatusType tableStatus = TableStatusType::UNKNOWN;

        /**
         * Creates date time
         */
        system_clock::time_point createdDateTime = system_clock::now();

        /**
         * Deletion protection
         */
        bool deletionProtectionEnabled{};

    private:
        friend DescribeTableResponse tag_invoke(boost::json::value_to_tag<DescribeTableResponse>, boost::json::value const &v) {
            DescribeTableResponse r = {};
            if (Core::Json::AttributeExists(v, "Table")) {
                const boost::json::object tableObject = v.at("Table").as_object();
                r.region = Core::Json::GetStringValue(tableObject, "Region");
                r.tableName = Core::Json::GetStringValue(tableObject, "TableName");
                r.tableArn = Core::Json::GetStringValue(tableObject, "TableArn");
                r.size = Core::Json::GetLongValue(tableObject, "TableSizeBytes");
                r.items = Core::Json::GetLongValue(tableObject, "ItemCount");
                r.provisionedThroughput = boost::json::value_to<ProvisionedThroughput>(tableObject, "ProvisionedThroughput");
                r.tableStatus = TableStatusTypeFromString(Core::Json::GetStringValue(tableObject, "TableStatus"));
                r.createdDateTime = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongValue(tableObject, "CreatedDateTime"));
                r.deletionProtectionEnabled = Core::Json::GetBoolValue(tableObject, "DeletionProtectionEnabled");
                if (Core::Json::AttributeExists(tableObject, "Tags")) {
                    r.tags = boost::json::value_to<std::vector<Tag> >(tableObject.at("Tags"));
                }
                if (Core::Json::AttributeExists(tableObject, "AttributeDefinitions")) {
                    r.attributeDefinitions = boost::json::value_to<std::vector<AttributeDefinition> >(tableObject.at("AttributeDefinitions"));
                }
                if (Core::Json::AttributeExists(tableObject, "KeySchema")) {
                    r.keySchema = boost::json::value_to<std::vector<KeySchema> >(tableObject.at("KeySchema"));
                }
                if (Core::Json::AttributeExists(tableObject, "TableClassSummary")) {
                    r.tableClassSummary = boost::json::value_to<TableClassSummary>(v.at("TableClassSummary"));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeTableResponse const &obj) {
            jv = {};
            boost::json::object tableObject = {
                {"Region", obj.region},
                {"TableName", obj.tableName},
                {"TableArn", obj.tableArn},
                {"TableSizeBytes", obj.size},
                {"ItemCount", obj.items},
                {"ProvisionedThroughput", boost::json::value_from(obj.provisionedThroughput)},
                {"TableStatus", TableStatusTypeToString(obj.tableStatus)},
                {"CreatedDateTime", Core::DateTimeUtils::UnixTimestamp(obj.createdDateTime)},
                {"DeletionProtectionEnabled", obj.deletionProtectionEnabled},
                {"TableClassSummary", boost::json::value_from(obj.tableClassSummary)},
            };
            if (!obj.tags.empty()) {
                tableObject["Tags"] = boost::json::value_from(obj.tags);
            }
            if (!obj.keySchema.empty()) {
                tableObject["KeySchema"] = boost::json::value_from(obj.keySchema);
            }
            if (!obj.attributeDefinitions.empty()) {
                tableObject["AttributeDefinitions"] = boost::json::value_from(obj.attributeDefinitions);
            }
            jv.as_object()["Table"] = tableObject;
        }
    };

} // namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H
