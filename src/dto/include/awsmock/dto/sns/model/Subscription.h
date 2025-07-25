//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_SUBSCRIPTIONS_H
#define AWSMOCK_DTO_SNS_SUBSCRIPTIONS_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include "MessageAttribute.h"
#include "awsmock/core/JsonUtils.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    struct Subscription final : Common::BaseCounter<Subscription> {

        /**
         * Subscription id
         */
        std::string id = Core::StringUtils::CreateRandomUuid();

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "id", id);
                Core::Bson::BsonUtils::SetStringValue(document, "region", region);
                Core::Bson::BsonUtils::SetStringValue(document, "topicArn", topicArn);
                Core::Bson::BsonUtils::SetStringValue(document, "protocol", protocol);
                Core::Bson::BsonUtils::SetStringValue(document, "subscriptionArn", subscriptionArn);
                Core::Bson::BsonUtils::SetStringValue(document, "endpoint", endpoint);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Subscription tag_invoke(boost::json::value_to_tag<Subscription>, boost::json::value const &v) {
            Subscription r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.protocol = Core::Json::GetStringValue(v, "protocol");
            r.subscriptionArn = Core::Json::GetStringValue(v, "subscriptionArn");
            r.endpoint = Core::Json::GetStringValue(v, "endpoint");
            r.owner = Core::Json::GetStringValue(v, "owner");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Subscription const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"id", obj.id},
                    {"topicArn", obj.topicArn},
                    {"protocol", obj.protocol},
                    {"subscriptionArn", obj.subscriptionArn},
                    {"endpoint", obj.endpoint},
                    {"owner", obj.owner},
            };
        }
    };

    typedef std::vector<Subscription> SubscriptionsList;

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_SUBSCRIPTIONS_H
