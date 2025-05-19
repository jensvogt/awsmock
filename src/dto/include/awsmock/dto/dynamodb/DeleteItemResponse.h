//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_RESPONSE_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include "model/AttributeValue.h"


#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;
    /**
     * @brief Delete item request DTO
     *
     * Example:
     * @code{.json}
     * {
     *   "ConsumedCapacity":
     *     {
     *       "TableName":"test-table",
     *       "CapacityUnits":1.0
     *    }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteItemResponse final : Common::BaseCounter<DeleteItemResponse> {

        /**
         * Item array
         */
        std::vector<std::map<std::string, AttributeValue>> attributes;

      private:

        friend DeleteItemResponse tag_invoke(boost::json::value_to_tag<DeleteItemResponse>, boost::json::value const &v) {
            DeleteItemResponse r;
            if (Core::Json::AttributeExists(v, "Attributes")) {
                for (boost::json::array itemsJson = v.at("Attributes").as_array(); auto &item: itemsJson) {
                    std::map<std::string, AttributeValue> itemMap;
                    for (auto &attribute: item.as_object()) {
                        itemMap[attribute.key()] = boost::json::value_to<AttributeValue>(attribute.value());
                    }
                    r.attributes.push_back(itemMap);
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteItemResponse const &obj) {
            if (!obj.attributes.empty()) {
                boost::json::array itemsJson;
                for (const auto &a: obj.attributes) {
                    boost::json::object itemJson;
                    for (const auto &[fst, snd]: a) {
                        itemJson[fst] = boost::json::value_from(snd);
                    }
                    itemsJson.emplace_back(itemJson);
                }
                jv.as_object()["Attributes"] = itemsJson;
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_RESPONSE_H
