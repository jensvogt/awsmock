//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/FunctionCounter.h>

namespace AwsMock::Dto::Lambda {

    struct ListFunctionCountersResponse final : Common::BaseCounter<ListFunctionCountersResponse> {

        /**
         * List of buckets
         */
        std::vector<FunctionCounter> functionCounters;

        /**
         * Total number of buckets
         */
        long total = 0;

      private:

        friend ListFunctionCountersResponse tag_invoke(boost::json::value_to_tag<ListFunctionCountersResponse>, boost::json::value const &v) {
            ListFunctionCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.functionCounters = boost::json::value_to<std::vector<FunctionCounter>>(v.at("functionCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListFunctionCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"functionCounters", boost::json::value_from(obj.functionCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H
