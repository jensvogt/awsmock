//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// Boost include<
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;

    /**
     * @brief Create table response DTO
     *
     * Example:
     * @code{.json}
     * {
     *   "TableDescription":
     *     {
     *       "AttributeDefinitions": [
     *           {
     *             "attributeName":"orgaNr",
     *             "AttributeType":"N"
     *           }
     *       ],
     *       "TableName":"test-table",
     *       "KeySchema":[
     *         {
     *           "attributeName":"orgaNr",
     *           "KeyType":"HASH"
     *         }
     *       ],
     *       "TableStatus":"ACTIVE",
     *       "CreationDateTime":1703158038.811,
     *       "ProvisionedThroughput":
     *         {
     *           "LastIncreaseDateTime":0.000,
     *           "LastDecreaseDateTime":0.000,
     *           "NumberOfDecreasesToday":0,
     *           "ReadCapacityUnits":1,
     *           "WriteCapacityUnits":1
     *         },
     *       "TableSizeBytes":0,
     *       "ItemCount":0,
     *       "TableArn":"arn:aws:dynamodb:ddblocal:000000000000:table/test-table",
     *       "DeletionProtectionEnabled":false
     *    }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateTableResponse final : Common::BaseCounter<CreateTableResponse> {

        /**
         * Table class
         */
        std::string tableClass;

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
        long tableSize = 0;

        /**
         * Item count
         */
        long itemCount = 0;

        /**
         * Delete protection enabled
         */
        bool deleteProtectionEnabled;

        /**
         * Key schema
         */
        std::vector<KeySchema> keySchemas;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Attribute definitions
         */
        std::vector<AttributeDefinition> attributeDefinitions;

        /**
         * Table status
         */
        TableStatusType tableStatus;

      private:

        friend CreateTableResponse tag_invoke(boost::json::value_to_tag<CreateTableResponse>, boost::json::value const &v) {
            CreateTableResponse r = {};
            if (Core::Json::AttributeExists(v, "TableDescription")) {
                const boost::json::object tableObject = v.at("Table").as_object();
                r.tableClass = Core::Json::GetStringValue(tableObject, "TableClass");
                r.tableName = Core::Json::GetStringValue(tableObject, "TableName");
                r.tableArn = Core::Json::GetStringValue(tableObject, "TableArn");
                r.itemCount = Core::Json::GetLongValue(tableObject, "ItemCount");
                r.tableStatus = TableStatusTypeFromString(Core::Json::GetStringValue(tableObject, "TableStatus"));
                if (Core::Json::AttributeExists(tableObject, "Tags")) {
                    r.tags = boost::json::value_to<std::map<std::string, std::string>>(tableObject.at("Tags"));
                }
                if (Core::Json::AttributeExists(tableObject, "AttributeDefinitions")) {
                    r.attributeDefinitions = boost::json::value_to<std::vector<AttributeDefinition>>(tableObject.at("AttributeDefinitions"));
                }
                if (Core::Json::AttributeExists(tableObject, "KeySchema")) {
                    r.keySchemas = boost::json::value_to<std::vector<KeySchema>>(tableObject.at("KeySchema"));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateTableResponse const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"TableArn", obj.tableArn},
                    {"TableSizeBytes", obj.tableSize},
                    {"ItemCount", obj.itemCount},
                    {"TableStatus", TableStatusTypeToString(obj.tableStatus)},
            };
            if (!obj.tags.empty()) {
                jv.as_object()["Tags"] = boost::json::value_from(obj.tags);
            }
            if (!obj.keySchemas.empty()) {
                jv.as_object()["KeySchema"] = boost::json::value_from(obj.keySchemas);
            }
            if (!obj.attributeDefinitions.empty()) {
                jv.as_object()["AttributeDefinitions"] = boost::json::value_from(obj.attributeDefinitions);
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H
