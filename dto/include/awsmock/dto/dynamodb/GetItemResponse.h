//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace Awsmock::Dto::DynamoDb {

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
        std::unordered_map<std::string, AttributeValue> attributes{};

      private:

        friend GetItemResponse tag_invoke(boost::json::value_to_tag<GetItemResponse>, boost::json::value const &v) {
            GetItemResponse r = {};
            if (Core::Json::AttributeExists(v, "Item")) {
                r.attributes = boost::json::value_to<std::unordered_map<std::string, AttributeValue>>(v.at("Item"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetItemResponse const &obj) {
            auto &o = jv.emplace_object();
            if (!obj.attributes.empty()) {
                o.emplace("Item", boost::json::value_from(obj.attributes));
            }
        }
    };

}// namespace Awsmock::Dto::DynamoDb
