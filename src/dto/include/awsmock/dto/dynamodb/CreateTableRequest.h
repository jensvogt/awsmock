//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb//model/StreamSpecification.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB create table request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateTableRequest final : Common::BaseCounter<CreateTableRequest> {

        /**
         * Table class
         */
        std::string tableClass;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Attribute definitions
         */
        std::vector<std::map<std::string, std::string>> attributes;

        /**
         * Key schemas
         */
        std::vector<std::map<std::string, std::string>> keySchemas;

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Tags
         */
        std::vector<std::map<std::string, std::string>> tags;

        /**
         * Streams
         */
        StreamSpecification streamSpecification;

        friend CreateTableRequest tag_invoke(boost::json::value_to_tag<CreateTableRequest>, boost::json::value const &v) {
            CreateTableRequest r;
            r.tableClass = Core::Json::GetStringValue(v, "TableClass");
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            if (Core::Json::AttributeExists(v, "ProvisionedThroughput")) {
                r.provisionedThroughput = boost::json::value_to<ProvisionedThroughput>(v.at("ProvisionedThroughput"));
            }
            if (Core::Json::AttributeExists(v, "StreamSpecification")) {
                r.streamSpecification = boost::json::value_to<StreamSpecification>(v.at("StreamSpecification"));
            }
            if (Core::Json::AttributeExists(v, "Tags")) {
                for (boost::json::array tagsArray = v.at("Tags").as_array(); const auto &a: tagsArray) {
                    boost::json::object tagObject = a.as_object();
                    std::map<std::string, std::string> tag;
                    tag["Key"] = a.at("Key").as_string();
                    tag["Value"] = a.at("Value").as_string();
                    r.tags.push_back(std::move(tag));
                }
            }
            if (Core::Json::AttributeExists(v, "AttributeDefinitions")) {
                for (boost::json::array attributeDefinitionsArray = v.at("AttributeDefinitions").as_array(); const auto &a: attributeDefinitionsArray) {
                    boost::json::object attributeDefinitionObject = a.as_object();
                    std::map<std::string, std::string> attributeDefinition;
                    attributeDefinition["AttributeName"] = a.at("AttributeName").as_string();
                    attributeDefinition["AttributeType"] = a.at("AttributeType").as_string();
                    r.attributes.push_back(std::move(attributeDefinition));
                }
            }
            if (Core::Json::AttributeExists(v, "KeySchema")) {
                for (boost::json::array attributeDefinitionsArray = v.at("KeySchema").as_array(); const auto &a: attributeDefinitionsArray) {
                    boost::json::object attributeDefinitionObject = a.as_object();
                    std::map<std::string, std::string> keySchema;
                    keySchema["AttributeName"] = a.at("AttributeName").as_string();
                    keySchema["KeyType"] = a.at("KeyType").as_string();
                    r.keySchemas.emplace_back(keySchema);
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateTableRequest const &obj) {
            jv = {
                    {"TableClass", obj.tableClass},
                    {"TableName", obj.tableName},
                    {"ProvisionedThroughput", boost::json::value_from(obj.provisionedThroughput)},
                    {"StreamSpecification", boost::json::value_from(obj.streamSpecification)},
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

#endif// AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_REQUEST_H
