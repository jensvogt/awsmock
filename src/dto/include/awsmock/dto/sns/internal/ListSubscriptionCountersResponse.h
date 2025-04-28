//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_SUBSCRIPTION_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_SUBSCRIPTION_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/Subscription.h>
#include <awsmock/dto/sns/model/SubscriptionCounter.h>

namespace AwsMock::Dto::SNS {

    struct ListSubscriptionCountersResponse final : Common::BaseCounter<ListSubscriptionCountersResponse> {

        /**
         * List of subscription counters
         */
        std::vector<SubscriptionCounter> subscriptionCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListSubscriptionCountersResponse tag_invoke(boost::json::value_to_tag<ListSubscriptionCountersResponse>, boost::json::value const &v) {
            ListSubscriptionCountersResponse r;
            r.total = v.at("total").as_int64();
            r.subscriptionCounters = boost::json::value_to<std::vector<SubscriptionCounter>>(v.at("subscriptionCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSubscriptionCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"subscriptionCounters", boost::json::value_from(obj.subscriptionCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_SUBSCRIPTION_COUNTERS_RESPONSE_H
