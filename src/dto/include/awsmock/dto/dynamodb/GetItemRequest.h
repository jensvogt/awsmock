//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_ITEM_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_GET_ITEM_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>
#include <awsmock/dto/dynamodb/model/ReturnConsumedCapacity.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get item request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetItemRequest final : Common::BaseCounter<GetItemRequest> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Keys
         */
        std::map<std::string, AttributeValue> keys;

        /**
         * Projection
         */
        std::vector<std::string> attributesToGet;

        /**
         * Projection exception
         */
        std::string projectionExpression;

        /**
         * Consistent read
         */
        bool consistentRead = false;

        /**
         * Return consumed capacity
         */
        ReturnConsumedCapacityType returnConsumedCapacity = ReturnConsumedCapacityType::NONE;

      private:

        friend GetItemRequest tag_invoke(boost::json::value_to_tag<GetItemRequest>, boost::json::value const &v) {
            GetItemRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.consistentRead = Core::Json::GetBoolValue(v, "ConsistentRead");
            r.projectionExpression = Core::Json::GetStringValue(v, "ProjectionExpression");
            r.returnConsumedCapacity = ReturnConsumedCapacityTypeFromString(Core::Json::GetStringValue(v, "ReturnConsumedCapacity"));

            if (Core::Json::AttributeExists(v, "AttributesToGet")) {
                r.attributesToGet = boost::json::value_to<std::vector<std::string>>(v.at("AttributesToGet"));
            }

            if (Core::Json::AttributeExists(v, "Key")) {
                for (auto &attribute: v.at("Key").as_object()) {
                    r.keys[attribute.key()] = boost::json::value_to<AttributeValue>(attribute.value());
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetItemRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"ConsistentRead", obj.consistentRead},
                    {"ReturnConsumedCapacity", ReturnConsumedCapacityTypeToString(obj.returnConsumedCapacity)},
                    {"Key", boost::json::value_from(obj.keys)},
            };
            if (!obj.projectionExpression.empty()) {
                jv.at("ProjectionExpression") = obj.projectionExpression;
            } else if (!obj.attributesToGet.empty()) {
                boost::json::array attributesToGetJson;
                for (auto &a: obj.attributesToGet) {
                    attributesToGetJson.emplace_back(a);
                }
                jv.as_object()["AttributesToGet"] = attributesToGetJson;
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_ITEM_REQUEST_H
