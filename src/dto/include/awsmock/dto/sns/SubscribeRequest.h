//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_SUBSCRIBE_REQUEST_H
#define AWSMOCK_DTO_SNS_SUBSCRIBE_REQUEST_H

// C++ standard includes
#include <string>

// Awsmock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct SubscribeRequest final : Common::BaseCounter<SubscribeRequest> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Protocol
         */
        std::string protocol;

        /**
         * Endpoint
         */
        std::string endpoint;

        /**
         * Return a subscription ARN flag
         */
        bool returnSubscriptionArn = true;

      private:

        friend SubscribeRequest tag_invoke(boost::json::value_to_tag<SubscribeRequest>, boost::json::value const &v) {
            SubscribeRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.protocol = Core::Json::GetStringValue(v, "protocol");
            r.endpoint = Core::Json::GetStringValue(v, "endpoint");
            r.returnSubscriptionArn = Core::Json::GetBoolValue(v, "returnSubscriptionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SubscribeRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"protocol", obj.protocol},
                    {"endpoint", obj.endpoint},
                    {"returnSubscriptionArn", obj.returnSubscriptionArn},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_SUBSCRIBE_REQUEST_H
