//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/ItemCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListItemCountersResponse final : Common::BaseCounter<ListItemCountersResponse> {

        /**
         * Items
         */
        std::vector<ItemCounter> itemCounters;

        /**
         * Total number of tables
         */
        long total = 0;

      private:

        friend ListItemCountersResponse tag_invoke(boost::json::value_to_tag<ListItemCountersResponse>, boost::json::value const &v) {
            ListItemCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.itemCounters = boost::json::value_to<std::vector<ItemCounter>>(v.at("itemCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListItemCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"itemCounters", boost::json::value_from(obj.itemCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_RESPONSE_H
