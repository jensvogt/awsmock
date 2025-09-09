//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_INSTANCE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_INSTANCE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/lambda/model/InstanceCounter.h>
#include <awsmock/dto/sqs/model/TagCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief List Lambda instance counter response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListLambdaInstanceCountersResponse final : Common::BaseCounter<ListLambdaInstanceCountersResponse> {

        /**
         * List of instance counters
         */
        std::vector<InstanceCounter> instanceCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListLambdaInstanceCountersResponse tag_invoke(boost::json::value_to_tag<ListLambdaInstanceCountersResponse>, boost::json::value const &v) {
            ListLambdaInstanceCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.instanceCounters = boost::json::value_to<std::vector<InstanceCounter>>(v.at("instanceCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaInstanceCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"instanceCounters", boost::json::value_from(obj.instanceCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_TAG_COUNTERS_RESPONSE_H
