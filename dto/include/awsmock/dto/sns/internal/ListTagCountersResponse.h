//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/TagCounter.h>

namespace Awsmock::Dto::SNS {

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
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "tagCounters")) {
                r.tagCounters = boost::json::value_to<std::vector<TagCounter>>(v.at("tagCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTagCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"tagCounters", boost::json::value_from(obj.tagCounters)},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
