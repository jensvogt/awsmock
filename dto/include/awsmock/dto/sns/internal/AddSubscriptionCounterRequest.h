//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_ADD_SUBSCRIPTION_COUNTER_REQUEST_H
#define AWSMOCK_DTO_SNS_ADD_SUBSCRIPTION_COUNTER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct AddSubscriptionCounterRequest final : Common::BaseCounter<AddSubscriptionCounterRequest> {

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
         * Owner
         */
        std::string owner;

    private:
        friend AddSubscriptionCounterRequest tag_invoke(boost::json::value_to_tag<AddSubscriptionCounterRequest>, boost::json::value const &v) {
            AddSubscriptionCounterRequest r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.protocol = Core::Json::GetStringValue(v, "protocol");
            r.endpoint = Core::Json::GetStringValue(v, "endpoint");
            r.owner = Core::Json::GetStringValue(v, "owner");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddSubscriptionCounterRequest const &obj) {
            jv = {
                {"region", obj.region},
                {"user", obj.user},
                {"requestId", obj.requestId},
                {"topicArn", obj.topicArn},
                {"protocol", obj.protocol},
                {"endpoint", obj.endpoint},
                {"owner", obj.owner},
            };
        }
    };

} // namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_ADD_SUBSCRIPTION_COUNTER_REQUEST_H
