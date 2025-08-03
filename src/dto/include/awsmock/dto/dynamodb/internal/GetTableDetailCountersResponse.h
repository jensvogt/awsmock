//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_TABLE_DETAIL_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_GET_TABLE_DETAIL_COUNTERS_RESPONSE_H

// C++ standard includes
#include <vector>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/TableCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get table details response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetTableDetailCountersResponse final : Common::BaseCounter<GetTableDetailCountersResponse> {

        /**
         * Table counters
         */
        TableCounter tableCounters;

      private:

        friend GetTableDetailCountersResponse tag_invoke(boost::json::value_to_tag<GetTableDetailCountersResponse>, boost::json::value const &v) {
            GetTableDetailCountersResponse r;
            r.tableCounters = boost::json::value_to<TableCounter>(v.at("tableCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetTableDetailCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"tableCounters", boost::json::value_from(obj.tableCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_TABLE_DETAIL_COUNTERS_RESPONSE_H
