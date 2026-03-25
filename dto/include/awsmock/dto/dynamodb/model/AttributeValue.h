//
// Created by vogje01 on 12/21/23.
//

#ifndef AWSMOCK_DTO_DYNAMODB_ATTRIBUTE_VALUE_H
#define AWSMOCK_DTO_DYNAMODB_ATTRIBUTE_VALUE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * DynamoDB attribute value
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AttributeValue final : Common::BaseCounter<AttributeValue> {

        /**
         * Type
         */
        std::string type;

        /**
         * Value
         */
        std::string attributeValue;

        /**
         * String value
         */
        std::string stringValue;

        /**
         * String set value
         */
        std::vector<std::string> stringSetValue;

        /**
         * Number value
         */
        std::string numberValue;

        /**
         * Number set value
         */
        std::vector<std::string> numberSetValue;

        /**
         * Boolean value
         */
        bool boolValue;

        /**
         * Binary value
         */
        std::vector<uint8_t> binaryValue;

        /**
         * Binary set value
         */
        std::vector<std::vector<uint8_t>> binarySetValue;

        /**
         * Null value
         */
        std::shared_ptr<bool> nullValue;

        /**
         * List value
         */
        std::vector<AttributeValue> listValue;

        /**
         * Map value
         */
        std::map<std::string, AttributeValue> mapValue;

      private:

        friend AttributeValue tag_invoke(boost::json::value_to_tag<AttributeValue>, boost::json::value const &v) {
            AttributeValue r = {};
            if (Core::Json::AttributeExists(v, "S")) {
                r.type = "S";
                r.stringValue = Core::Json::GetStringValue(v, "S");
            } else if (Core::Json::AttributeExists(v, "SS")) {
                r.type = "SS";
                r.stringSetValue = boost::json::value_to<std::vector<std::string>>(v.at("SS"));
            } else if (Core::Json::AttributeExists(v, "N")) {
                r.type = "N";
                r.numberValue = Core::Json::GetStringValue(v, "N");
            } else if (Core::Json::AttributeExists(v, "NS")) {
                r.type = "NS";
                r.numberSetValue = boost::json::value_to<std::vector<std::string>>(v.at("NS"));
            } else if (Core::Json::AttributeExists(v, "L")) {
                r.type = "L";
                r.listValue = boost::json::value_to<std::vector<AttributeValue>>(v.at("L"));
            } else if (Core::Json::AttributeExists(v, "M")) {
                r.type = "M";
                r.mapValue = boost::json::value_to<std::map<std::string, AttributeValue>>(v.at("M"));
            } else if (Core::Json::AttributeExists(v, "BOOL")) {
                r.type = "BOOL";
                r.boolValue = Core::Json::GetBoolValue(v, "BOOL");
            } else if (Core::Json::AttributeExists(v, "NULL")) {
                r.type = "NULL";
                r.nullValue = std::make_shared<bool>(Core::Json::GetBoolValue(v, "NULL"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AttributeValue const &obj) {
            if (obj.type == "S") {
                jv = {{"S", obj.stringValue}};
            } else if (obj.type == "N") {
                jv = {{"N", obj.numberValue}};
            } else if (obj.type == "SS") {
                jv = {{"SS", boost::json::value_from(obj.stringSetValue)}};
            } else if (obj.type == "NS") {
                jv = {{"SS", boost::json::value_from(obj.numberSetValue)}};
            } else if (obj.type == "L") {
                jv = {{"L", boost::json::value_from(obj.listValue)}};
            } else if (obj.type == "M") {
                jv = {{"M", boost::json::value_from(obj.mapValue)}};
            } else if (obj.type == "BOOL" && obj.boolValue) {
                jv = {{"BOOL", obj.boolValue}};
            } else if (obj.type == "NULL" && obj.nullValue) {
                jv = {{"NULL", *obj.nullValue}};
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_ATTRIBUTE_VALUE_H
