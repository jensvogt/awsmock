//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_TAG_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_TAG_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/TagCounter.h>

namespace AwsMock::Dto::SQS {

    struct ListQueueTagCountersResponse final : Common::BaseCounter<ListQueueTagCountersResponse> {

        /**
         * List of tag counters
         */
        std::vector<TagCounter> tagCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListQueueTagCountersResponse tag_invoke(boost::json::value_to_tag<ListQueueTagCountersResponse>, boost::json::value const &v) {
            ListQueueTagCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "tagCounters")) {
                r.tagCounters = boost::json::value_to<std::vector<TagCounter>>(v.at("tagCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListQueueTagCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"tagCounters", boost::json::value_from(obj.tagCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_TAG_COUNTERS_RESPONSE_H
