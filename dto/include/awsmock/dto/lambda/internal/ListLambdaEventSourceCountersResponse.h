//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/EventSourceMapping.h>

namespace AwsMock::Dto::Lambda {
    /**
     * @brief List Lambda event sources response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListLambdaEventSourceCountersResponse final : Common::BaseCounter<ListLambdaEventSourceCountersResponse> {

        /**
         * List of tag counters
         */
        std::vector<EventSourceMapping> eventSourceCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListLambdaEventSourceCountersResponse tag_invoke(boost::json::value_to_tag<ListLambdaEventSourceCountersResponse>, boost::json::value const &v) {
            ListLambdaEventSourceCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.eventSourceCounters = boost::json::value_to<std::vector<EventSourceMapping>>(v.at("eventSourceCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaEventSourceCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"eventSourceCounters", boost::json::value_from(obj.eventSourceCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_COUNTERS_RESPONSE_H
