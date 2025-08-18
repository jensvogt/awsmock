//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>

namespace AwsMock::Dto::SQS {

    struct ListParameterCountersResponse final : Common::BaseCounter<ListParameterCountersResponse> {

        /**
         * List of queues ARNs
         */
        std::vector<QueueCounter> parameterCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListParameterCountersResponse tag_invoke(boost::json::value_to_tag<ListParameterCountersResponse>, boost::json::value const &v) {
            ListParameterCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "queueCounters")) {
                r.parameterCounters = boost::json::value_to<std::vector<QueueCounter>>(v.at("queueCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListParameterCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"queueCounters", boost::json::value_from(obj.parameterCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_COUNTERS_RESPONSE_H
