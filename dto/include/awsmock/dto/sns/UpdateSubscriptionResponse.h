//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMOck includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SNS {

    struct UpdateSubscriptionResponse final : Common::BaseCounter<UpdateSubscriptionResponse> {

        /**
         * @brief Subscription ARN
         */
        std::string subscriptionArn;

      private:

        friend UpdateSubscriptionResponse tag_invoke(boost::json::value_to_tag<UpdateSubscriptionResponse>, boost::json::value const &v) {
            UpdateSubscriptionResponse r;
            r.subscriptionArn = Core::Json::GetStringValue(v, "subscriptionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSubscriptionResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"subscriptionArn", obj.subscriptionArn},
            };
        }
    };

}// namespace Awsmock::Dto::SNS
