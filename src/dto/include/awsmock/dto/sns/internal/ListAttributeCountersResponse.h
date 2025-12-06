//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_ATTRIBUTE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_ATTRIBUTE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/AttributeCounter.h>

namespace AwsMock::Dto::SNS {

    struct ListAttributeCountersResponse final : Common::BaseCounter<ListAttributeCountersResponse> {

        /**
         * List of attribute counters
         */
        std::vector<AttributeCounter> attributeCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListAttributeCountersResponse tag_invoke(boost::json::value_to_tag<ListAttributeCountersResponse>, boost::json::value const &v) {
            ListAttributeCountersResponse r;
            r.total = v.at("total").as_int64();
            r.attributeCounters = boost::json::value_to<std::vector<AttributeCounter>>(v.at("attributeCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListAttributeCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"attributeCounters", boost::json::value_from(obj.attributeCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_ATTRIBUTE_COUNTERS_RESPONSE_H
