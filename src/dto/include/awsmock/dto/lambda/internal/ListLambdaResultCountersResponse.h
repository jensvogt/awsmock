//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_LAMBDA_RESULT_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_LAMBDA_RESULT_COUNTERS_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/LambdaResultCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief List Lambda result counter response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListLambdaResultCountersResponse final : Common::BaseCounter<ListLambdaResultCountersResponse> {

        /**
         * List of buckets
         */
        std::vector<LambdaResultCounter> lambdaResultCounters;

        /**
         * Total number of buckets
         */
        long long total = 0;

      private:

        friend ListLambdaResultCountersResponse tag_invoke(boost::json::value_to_tag<ListLambdaResultCountersResponse>, boost::json::value const &v) {
            ListLambdaResultCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.lambdaResultCounters = boost::json::value_to<std::vector<LambdaResultCounter>>(v.at("lambdaResultCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaResultCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"lambdaResultCounters", boost::json::value_from(obj.lambdaResultCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_LAMBDA_RESULT_COUNTERS_RESPONSE_H
