//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/TopicCounter.h>

namespace AwsMock::Dto::SNS {

    struct ListTopicCountersResponse final : Common::BaseCounter<ListTopicCountersResponse> {

        /**
         * List of topic counters
         */
        std::vector<TopicCounter> topicCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListTopicCountersResponse tag_invoke(boost::json::value_to_tag<ListTopicCountersResponse>, boost::json::value const &v) {
            ListTopicCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "topicCounters")) {
                r.topicCounters = boost::json::value_to<std::vector<TopicCounter>>(v.at("topicCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTopicCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"topicCounters", boost::json::value_from(obj.topicCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_RESPONSE_H
