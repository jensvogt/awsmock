//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>

namespace AwsMock::Dto::SQS {

    struct ListQueueCountersResponse final : Common::BaseCounter<ListQueueCountersResponse> {

        /**
         * List of queues ARNs
         */
        std::vector<QueueCounter> queueCounters;

        /**
         * Total number of queues
         */
        long total;

      private:

        friend ListQueueCountersResponse tag_invoke(boost::json::value_to_tag<ListQueueCountersResponse>, boost::json::value const &v) {
            ListQueueCountersResponse r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.total = v.at("total").as_int64();
            r.queueCounters = boost::json::value_to<std::vector<QueueCounter>>(v.at("queueCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueueCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"queueCounters", boost::json::value_from(obj.queueCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_COUNTERS_RESPONSE_H
