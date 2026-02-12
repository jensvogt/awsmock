//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_ITEM_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_GET_ITEM_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// Boost include<
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/Item.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;

    /**
     * @brief DynamoDB Get item response
     *
     * Example:
     * @code{.json}
     * {
     *   "Item":
     *     {
     *       "orgaNr":{
     *         "N":"72"
     *       }
     *     },
     *     "ConsumedCapacity":
     *       {
     *         "TableName":"test-table",
     *         "CapacityUnits":0.5
     *       }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetItemResponse final : Common::BaseCounter<GetItemResponse> {

        /**
         * Attributes
         */
        std::map<std::string, AttributeValue> attributes;

      private:

        friend GetItemResponse tag_invoke(boost::json::value_to_tag<GetItemResponse>, boost::json::value const &v) {
            GetItemResponse r = {};
            if (Core::Json::AttributeExists(v, "Item")) {
                r.attributes = boost::json::value_to<std::map<std::string, AttributeValue>>(v.at("Item"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetItemResponse const &obj) {
            jv = boost::json::object();
            if (!obj.attributes.empty()) {
                jv.as_object().emplace("Item", boost::json::value_from(obj.attributes));
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_ITEM_RESPONSE_H
