//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// Boost include<
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>
#include <awsmock/dto/dynamodb/model/TableClassSummary.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>

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
        long tableSize{};

        /**
         * Item count
         */
        long itemCount{};

        /**
         * Key schema
         */
        std::vector<std::map<std::string, std::string>> keySchemas;

        /**
         * Attribute definitions
         */
        std::vector<std::map<std::string, std::string>> attributes;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Table class summary
         */
        TableClassSummary tableClassSummary;

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
                r.tableSize = Core::Json::GetLongValue(tableObject, "TableSizeBytes");
                r.itemCount = Core::Json::GetLongValue(tableObject, "ItemCount");
                r.provisionedThroughput = boost::json::value_to<ProvisionedThroughput>(tableObject, "ProvisionedThroughput");
                r.tableStatus = TableStatusTypeFromString(Core::Json::GetStringValue(tableObject, "TableStatus"));
                r.createdDateTime = Core::DateTimeUtils::FromUnixTimestamp(Core::Json::GetLongValue(tableObject, "CreatedDateTime"));
                r.deletionProtectionEnabled = Core::Json::GetBoolValue(tableObject, "DeletionProtectionEnabled");
                if (Core::Json::AttributeExists(tableObject, "Tags")) {
                    r.tags = boost::json::value_to<std::map<std::string, std::string>>(tableObject.at("Tags"));
                }
                if (Core::Json::AttributeExists(tableObject, "AttributeDefinitions")) {
                    for (boost::json::array attributeDefinitionsArray = tableObject.at("AttributeDefinitions").as_array(); const auto &a: attributeDefinitionsArray) {
                        boost::json::object attributeDefinitionObject = a.as_object();
                        std::map<std::string, std::string> attributeDefinition;
                        attributeDefinition["AttributeName"] = a.at("AttributeName").as_string();
                        attributeDefinition["AttributeType"] = a.at("AttributeType").as_string();
                        r.attributes.push_back(std::move(attributeDefinition));
                    }
                }
                if (Core::Json::AttributeExists(tableObject, "KeySchema")) {
                    for (boost::json::array attributeDefinitionsArray = tableObject.at("KeySchema").as_array(); const auto &a: attributeDefinitionsArray) {
                        boost::json::object attributeDefinitionObject = a.as_object();
                        std::map<std::string, std::string> keySchema;
                        keySchema["AttributeName"] = a.at("AttributeName").as_string();
                        keySchema["KeyType"] = a.at("KeyType").as_string();
                        r.keySchemas.emplace_back(keySchema);
                    }
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
                    {"TableSizeBytes", obj.tableSize},
                    {"ItemCount", obj.itemCount},
                    {"ProvisionedThroughput", boost::json::value_from(obj.provisionedThroughput)},
                    {"TableStatus", TableStatusTypeToString(obj.tableStatus)},
                    {"CreatedDateTime", Core::DateTimeUtils::UnixTimestamp(obj.createdDateTime)},
                    {"DeletionProtectionEnabled", obj.deletionProtectionEnabled},
                    {"TableClassSummary", boost::json::value_from(obj.tableClassSummary)},
            };
            if (!obj.tags.empty()) {
                tableObject["Tags"] = boost::json::value_from(obj.tags);
            }
            if (!obj.keySchemas.empty()) {
                tableObject["KeySchema"] = boost::json::value_from(obj.keySchemas);
            }
            if (!obj.attributes.empty()) {
                tableObject["AttributeDefinitions"] = boost::json::value_from(obj.attributes);
            }
            jv.as_object()["Table"] = tableObject;
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H
