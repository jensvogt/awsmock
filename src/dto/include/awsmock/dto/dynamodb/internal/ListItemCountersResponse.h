//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// Boost include<
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
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
            r.region = v.at("region").as_string();
            r.requestId = v.at("requestId").as_string();
            r.user = v.at("user").as_string();
            r.total = v.at("total").as_int64();

            // Bucket counters
            if (v.as_object().contains("bucketCounters")) {
                for (const auto &o: v.at("bucketCounters").as_array()) {
                    r.itemCounters.emplace_back(boost::json::value_to<ItemCounter>(o));
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListItemCountersResponse const &obj) {
            // Bucket counters
            boost::json::array bucketCounters;
            if (!obj.itemCounters.empty()) {
                for (const auto &o: obj.itemCounters) {
                    bucketCounters.emplace_back(boost::json::value_from(o));
                }
            }
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"bucketCounters", bucketCounters},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_RESPONSE_H
