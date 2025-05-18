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
        std::vector<std::map<std::string, std::string>> keySchemas;

        /**
         * Tags
         */
        std::vector<std::map<std::string, std::string>> tags;

        /**
         * Attribute definitions
         */
        std::vector<std::map<std::string, std::string>> attributes;

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
                    r.tags = boost::json::value_to<std::vector<std::map<std::string, std::string>>>(tableObject, "Tags");
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
                boost::json::array jvKeySchema;
                for (const auto &k: obj.keySchemas) {
                    boost::json::object jvKeySchemaItem;
                    jvKeySchemaItem["AttributeName"] = k.at("AttributeName");
                    jvKeySchemaItem["KeyType"] = k.at("KeyType");
                    jvKeySchema.push_back(std::move(jvKeySchemaItem));
                }
                jv.as_object()["KeySchema"] = jvKeySchema;
            }
            if (!obj.attributes.empty()) {
                boost::json::array jvAttributeArray;
                for (const auto &a: obj.attributes) {
                    boost::json::object jvAttributeItem;
                    jvAttributeItem["AttributeName"] = a.at("AttributeName");
                    jvAttributeItem["AttributeType"] = a.at("AttributeType");
                    jvAttributeArray.push_back(std::move(jvAttributeItem));
                }
                jv.as_object()["AttributeDefinitions"] = jvAttributeArray;
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H
