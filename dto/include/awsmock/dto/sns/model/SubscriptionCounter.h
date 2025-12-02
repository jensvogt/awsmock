//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_SUBSCRIPTION_COUNTER_H
#define AWSMOCK_DTO_SNS_SUBSCRIPTION_COUNTER_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct SubscriptionCounter final : Common::BaseCounter<SubscriptionCounter> {

        /**
         * Subscription id
         */
        std::string id;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Protocol
         */
        std::string protocol;

        /**
         * Subscription ARN
         */
        std::string subscriptionArn;

        /**
         * Endpoint
         */
        std::string endpoint;

        /**
         * Owner
         */
        std::string owner;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend SubscriptionCounter tag_invoke(boost::json::value_to_tag<SubscriptionCounter>, boost::json::value const &v) {
            SubscriptionCounter r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.protocol = Core::Json::GetStringValue(v, "protocol");
            r.subscriptionArn = Core::Json::GetStringValue(v, "subscriptionArn");
            r.endpoint = Core::Json::GetStringValue(v, "endpoint");
            r.owner = Core::Json::GetStringValue(v, "owner");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SubscriptionCounter const &obj) {
            jv = {
                    {"id", obj.id},
                    {"topicArn", obj.topicArn},
                    {"protocol", obj.protocol},
                    {"subscriptionArn", obj.subscriptionArn},
                    {"endpoint", obj.endpoint},
                    {"owner", obj.owner},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_SUBSCRIPTION_COUNTER_H
