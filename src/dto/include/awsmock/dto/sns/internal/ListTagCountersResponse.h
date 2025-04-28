//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_TAG_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_TAG_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/TagCounter.h>

namespace AwsMock::Dto::SNS {

    struct ListTagCountersResponse final : Common::BaseCounter<ListTagCountersResponse> {

        /**
         * List of tag counters
         */
        std::vector<TagCounter> tagCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListTagCountersResponse tag_invoke(boost::json::value_to_tag<ListTagCountersResponse>, boost::json::value const &v) {
            ListTagCountersResponse r;
            r.total = v.at("total").as_int64();
            r.tagCounters = boost::json::value_to<std::vector<TagCounter>>(v.at("tagCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTagCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"tagCounters", boost::json::value_from(obj.tagCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_TAG_COUNTERS_RESPONSE_H
