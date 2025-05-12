//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_LAMBDA_TRIGGER_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_LAMBDA_TRIGGER_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/LambdaTriggerCounter.h>

namespace AwsMock::Dto::SQS {

    struct ListLambdaTriggerCountersResponse final : Common::BaseCounter<ListLambdaTriggerCountersResponse> {

        /**
         * List of lambda trigger counters
         */
        std::vector<LambdaTriggerCounter> lambdaTriggerCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListLambdaTriggerCountersResponse tag_invoke(boost::json::value_to_tag<ListLambdaTriggerCountersResponse>, boost::json::value const &v) {
            ListLambdaTriggerCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "lambdaTriggerCounters")) {
                r.lambdaTriggerCounters = boost::json::value_to<std::vector<LambdaTriggerCounter>>(v.at("lambdaTriggerCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaTriggerCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"lambdaTriggerCounters", boost::json::value_from(obj.lambdaTriggerCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SNS_LIST_LAMBDA_TRIGGER_COUNTERS_RESPONSE_H
