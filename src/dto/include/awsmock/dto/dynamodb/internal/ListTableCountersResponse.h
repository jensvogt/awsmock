//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <vector>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/TableCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableCountersResponse final : Common::BaseCounter<ListTableCountersResponse> {

        /**
         * Table names
         */
        std::vector<TableCounter> tableCounters;

        /**
         * Total number of tables
         */
        long total = 0;

      private:

        friend ListTableCountersResponse tag_invoke(boost::json::value_to_tag<ListTableCountersResponse>, boost::json::value const &v) {
            ListTableCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.tableCounters = boost::json::value_to<std::vector<TableCounter>>(v.at("tableCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTableCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"tableCounters", boost::json::value_from(obj.tableCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_RESPONSE_H
