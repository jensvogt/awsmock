//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_QUERY_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_QUERY_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Query request
     *
     * Example:
     * @code{.json}
     * {
     *   "ConsumedCapacity":
     *     {
     *       "TableName":"test-table",
     *       "CapacityUnits":1.0
     *     }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct QueryResponse final : Common::BaseCounter<QueryResponse> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Item array
         */
        std::vector<std::map<std::string, AttributeValue>> items;

      private:

        friend QueryResponse tag_invoke(boost::json::value_to_tag<QueryResponse>, boost::json::value const &v) {
            QueryResponse r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            if (Core::Json::AttributeExists(v, "Items")) {
                for (boost::json::array itemsJson = v.at("Items").as_array(); auto &item: itemsJson) {
                    std::map<std::string, AttributeValue> itemMap;
                    for (auto &attribute: item.as_object()) {
                        itemMap[attribute.key()] = boost::json::value_to<AttributeValue>(attribute.value());
                    }
                    r.items.push_back(itemMap);
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, QueryResponse const &obj) {
            boost::json::array itemsJson;
            if (!obj.items.empty()) {
                for (const auto &a: obj.items) {
                    boost::json::object itemJson;
                    for (const auto &[fst, snd]: a) {
                        itemJson[fst] = boost::json::value_from(snd);
                    }
                    itemsJson.emplace_back(itemJson);
                }
            }
            jv = {
                    {"TableName", obj.tableName},
                    {"Items", itemsJson},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_QUERY_RESPONSE_H
