//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_GET_SUBSCRIPTION_COUNTER_RESPONSE_H
#define AWSMOCK_DTO_SNS_GET_SUBSCRIPTION_COUNTER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/Subscription.h>
#include <awsmock/dto/sns/model/SubscriptionCounter.h>

namespace AwsMock::Dto::SNS {

    struct GetSubscriptionCounterResponse final : Common::BaseCounter<GetSubscriptionCounterResponse> {

        /**
         * List of subscription counters
         */
        SubscriptionCounter subscriptionCounter;

    private:
        friend GetSubscriptionCounterResponse tag_invoke(boost::json::value_to_tag<GetSubscriptionCounterResponse>, boost::json::value const &v) {
            GetSubscriptionCounterResponse r;
            r.subscriptionCounter = boost::json::value_to<SubscriptionCounter>(v.at("subscriptionCounter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetSubscriptionCounterResponse const &obj) {
            jv = {
                {"subscriptionCounter", boost::json::value_from(obj.subscriptionCounter)},
            };
        }
    };

} // namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_GET_SUBSCRIPTION_COUNTER_RESPONSE_H
