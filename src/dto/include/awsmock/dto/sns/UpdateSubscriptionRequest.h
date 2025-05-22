//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_UPDATE_SUBSCRIPTION_REQUEST_H
#define AWSMOCK_DTO_SNS_UPDATE_SUBSCRIPTION_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// Awsmock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct UpdateSubscriptionRequest final : Common::BaseCounter<UpdateSubscriptionRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Subscription ARN
         */
        std::string subscriptionArn;

        /**
         * Protocol
         */
        std::string protocol;

        /**
         * Endpoint
         */
        std::string endpoint;

        /**
         * Owner
         */
        std::string owner;

    private:

        friend UpdateSubscriptionRequest tag_invoke(boost::json::value_to_tag<UpdateSubscriptionRequest>, boost::json::value const &v) {
            UpdateSubscriptionRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.subscriptionArn = Core::Json::GetStringValue(v, "subscriptionArn");
            r.protocol = Core::Json::GetStringValue(v, "protocol");
            r.endpoint = Core::Json::GetStringValue(v, "endpoint");
            r.owner = Core::Json::GetStringValue(v, "owner");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSubscriptionRequest const &obj) {
            jv = {
                {"region", obj.region},
                {"user", obj.user},
                {"requestId", obj.requestId},
                {"topicArn", obj.topicArn},
                {"subscriptionArn", obj.subscriptionArn},
                {"protocol", obj.protocol},
                {"endpoint", obj.endpoint},
                {"owner", obj.owner},
        };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_UPDATE_SUBSCRIPTION_REQUEST_H
