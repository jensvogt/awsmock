//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    struct ExpressionAttributeValue {
        std::map<std::string, std::string> expressionAttributeValues;

      private:

        friend ExpressionAttributeValue tag_invoke(boost::json::value_to_tag<ExpressionAttributeValue>, boost::json::value const &v) {
            ExpressionAttributeValue r;
            r.expressionAttributeValues = boost::json::value_to<std::map<std::string, std::string>>(v);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ExpressionAttributeValue const &obj) {
            jv = {};
            for (const auto &[k, v]: obj.expressionAttributeValues)
                jv.as_object()[k] = v;
        }
    };

    /**
     * @brief Query request
     *
     * @code{.json}
     * {
     *    "TableName" : "pim_local_compliance_record",
     *    "Limit" : 1,
     *    "ScanIndexForward" : false,
     *    "KeyConditionExpression" : "#AMZN_MAPPED_composedPartitionKey = :AMZN_MAPPED_composedPartitionKey",
     *    "ExpressionAttributeNames" : {
     *      "#AMZN_MAPPED_composedPartitionKey" : "composedPartitionKey"
     *    },
     *    "ExpressionAttributeValues" : {
     *      ":AMZN_MAPPED_composedPartitionKey" : {
     *        "S" : "DEMODLI###004d9ca8213f42b2b80d2cc8fc0ecce3###EUROPEAN_ACCESSIBILITY_ACT"
     *      }
     *    }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct QueryRequest final : Common::BaseCounter<QueryRequest> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Limit
         */
        int limit{};

        /**
         * Scan forward
         */
        bool scanIndexForward{};

        /**
         * Scan forward
         */
        std::string keyConditionExpression{};

        /**
         * Expression attribute names
         */
        std::map<std::string, std::string> expressionAttributeNames{};

        /**
         * Expression attribute values
         */
        std::map<std::string, ExpressionAttributeValue> expressionAttributeValues{};

      private:

        friend QueryRequest tag_invoke(boost::json::value_to_tag<QueryRequest>, boost::json::value const &v) {
            QueryRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.limit = Core::Json::GetIntValue(v, "Limit");
            r.scanIndexForward = Core::Json::GetBoolValue(v, "ScanIndexForward");
            r.keyConditionExpression = Core::Json::GetStringValue(v, "KeyConditionExpression");
            if (Core::Json::AttributeExists(v, "ExpressionAttributeNames")) {
                r.expressionAttributeNames = boost::json::value_to<std::map<std::string, std::string>>(v.at("ExpressionAttributeNames"));
            }
            if (Core::Json::AttributeExists(v, "ExpressionAttributeValues")) {
                r.expressionAttributeValues = boost::json::value_to<std::map<std::string, ExpressionAttributeValue>>(v.at("ExpressionAttributeValues"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, QueryRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"TableName", obj.tableName},
                    {"Limit", obj.limit},
                    {"ScanIndexForward", obj.scanIndexForward},
                    {"KeyConditionExpression", obj.keyConditionExpression},
                    {"ExpressionAttributeNames", boost::json::value_from(obj.expressionAttributeNames)},
                    {"ExpressionAttributeValues", boost::json::value_from(obj.expressionAttributeValues)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_QUERY_REQUEST_H
