//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

    struct GetSubscriptionCounterRequest final : Common::BaseCounter<GetSubscriptionCounterRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Subscription ID
         */
        std::string subscriptionArn;

      private:

        friend GetSubscriptionCounterRequest tag_invoke(boost::json::value_to_tag<GetSubscriptionCounterRequest>, boost::json::value const &v) {
            GetSubscriptionCounterRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.subscriptionArn = Core::Json::GetStringValue(v, "subscriptionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetSubscriptionCounterRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"subscriptionArn", obj.subscriptionArn},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
