//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include "model/ConsumedCapacity.h"


#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::DynamoDb {

    /**
     * @brief Put item response
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
    struct PutItemResponse final : Common::BaseCounter<PutItemResponse> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Table item
         */
        Item item;

        /**
         * Table item
         */
        ConsumedCapacity consumedCapacity;

      private:

        friend PutItemResponse tag_invoke(boost::json::value_to_tag<PutItemResponse>, boost::json::value const &v) {
            PutItemResponse r{};
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            if (Core::Json::AttributeExists(v, "Attributes")) {
                r.item = boost::json::value_to<Item>(v.at("Attributes"));
            }
            if (Core::Json::AttributeExists(v, "ConsumedCapacity")) {
                r.consumedCapacity = boost::json::value_to<ConsumedCapacity>(v.at("ConsumedCapacity"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutItemResponse const &obj) {
            boost::json::object itemJson;
            for (const auto &[fst, snd]: obj.item.attributes) {
                itemJson[fst] = boost::json::value_from(snd);
            }
            jv = {
                    {"TableName", obj.tableName},
                    {"Item", boost::json::value_from(obj.item.attributes)},
                    {"ConsumedCapacity", boost::json::value_from(obj.consumedCapacity)},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
