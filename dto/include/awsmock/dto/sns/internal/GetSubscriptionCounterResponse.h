//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/Subscription.h>
#include <awsmock/dto/sns/model/SubscriptionCounter.h>

namespace Awsmock::Dto::SNS {

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

}// namespace Awsmock::Dto::SNS
