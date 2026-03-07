//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_REQUEST_H

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb//model/StreamSpecification.h>
#include <awsmock/dto/dynamodb/model/AttributeDefinition.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>
#include <awsmock/dto/dynamodb/model/KeySchema.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>
#include <awsmock/dto/dynamodb/model/Tag.h>

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
        std::vector<AttributeDefinition> attributes;

        /**
         * Key schemas
         */
        std::vector<KeySchema> keySchema;

        /**
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Tags
         */
        std::vector<Tag> tags;

        /**
         * Streams
         */
        StreamSpecification streamSpecification;

      private:

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
                r.tags = boost::json::value_to<std::vector<Tag>>(v.at("Tags"));
            }
            if (Core::Json::AttributeExists(v, "AttributeDefinitions")) {
                r.attributes = boost::json::value_to<std::vector<AttributeDefinition>>(v.at("AttributeDefinitions"));
            }
            if (Core::Json::AttributeExists(v, "KeySchema")) {
                r.keySchema = boost::json::value_to<std::vector<KeySchema>>(v.at("KeySchema"));
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
            if (!obj.keySchema.empty()) {
                jv.as_object()["KeySchema"] = boost::json::value_from(obj.keySchema);
            }
            if (!obj.attributes.empty()) {
                jv.as_object()["AttributeDefinitions"] = boost::json::value_from(obj.attributes);
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_REQUEST_H
