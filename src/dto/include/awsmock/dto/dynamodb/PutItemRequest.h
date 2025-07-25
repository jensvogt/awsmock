//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_PUT_ITEM_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_PUT_ITEM_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include "model/ReturnConsumedCapacity.h"


#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>
#include <awsmock/dto/dynamodb/model/ReturnValues.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB put item request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutItemRequest final : Common::BaseCounter<PutItemRequest> {

        /**
         * The name of the table to contain the item.
         *
         * @par
         * Pattern: [a-zA-Z0-9_.-]+
         */
        std::string tableName;

        /**
         * Attributes
         *
         * @par
         * A map of attribute name/value pairs, one for each attribute. Only the primary key attributes are required; you
         * can optionally provide other attribute name-value pairs for the item.
         *
         * @par
         * You must provide all of the attributes for the primary key. For example, with a simple primary key, you only
         * need to provide a value for the partition key. For a composite primary key, you must provide both values for
         * both the partition key and the sort key.
         */
        std::map<std::string, AttributeValue> attributes;

        /**
         * Keys
         *
         * @par
         * A map of attribute name/value pairs, one for each attribute. Only the primary key attributes are required; you
         * can optionally provide other attribute name-value pairs for the item.
         *
         * @par
         * You must provide all of the attributes for the primary key. For example, with a simple primary key, you only
         * need to provide a value for the partition key. For a composite primary key, you must provide both values for
         * both the partition key and the sort key.
         */
        std::map<std::string, AttributeValue> keys;

        /**
         * Conditional expression
         *
         * @par
         * A condition that must be satisfied in order for a conditional PutItem operation to succeed.
         */
        std::string conditionalExpression;

        /**
         * Return consumed capacity.
         *
         * @par
         * Determines the level of detail about either provisioned or on-demand throughput consumption that is returned in
         * the response: INDEXES | TOTAL | NONE
         */
        ReturnConsumedCapacityType returnConsumedCapacity = ReturnConsumedCapacityType::NONE;

        /**
         * Return values
         *
         * @par
         * Use ReturnValues if you want to get the item attributes as they appeared before they were updated with the
         * PutItem request. For PutItem, the valid values are: NONE | ALL_OLD.
         */
        ReturnValuesType returnValues = ReturnValuesType::NONE;

        /**
         * Original HTTP request body.
         *
         * @par
         * Needed for the request to the docker image.
         */
        std::string body;

        /**
         * @brief Original HTTP request headers.
         *
         * @par
         * Needed for the request to the docker image.
         */
        std::map<std::string, std::string> headers;

      private:

        friend PutItemRequest tag_invoke(boost::json::value_to_tag<PutItemRequest>, boost::json::value const &v) {
            PutItemRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.conditionalExpression = Core::Json::GetStringValue(v, "ConditionalExpression");
            r.returnValues = ReturnValuesTypeFromString(Core::Json::GetStringValue(v, "ReturnValues"));
            r.returnConsumedCapacity = ReturnConsumedCapacityTypeFromString(Core::Json::GetStringValue(v, "ReturnConsumedCapacity"));

            if (Core::Json::AttributeExists(v, "Item")) {
                for (auto &attribute: v.at("Item").as_object()) {
                    r.attributes[attribute.key()] = boost::json::value_to<AttributeValue>(attribute.value());
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutItemRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"ReturnValues", ReturnValuesTypeToString(obj.returnValues)},
                    {"ReturnConsumedCapacity", ReturnConsumedCapacityTypeToString(obj.returnConsumedCapacity)},
                    {"Item", boost::json::value_from(obj.attributes)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_PUT_ITEM_REQUEST_H
