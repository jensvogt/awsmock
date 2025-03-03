//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_SUBSCRIPTIONS_H
#define AWSMOCK_DTO_SNS_SUBSCRIPTIONS_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::SNS {

    struct Subscription {

        /**
         * Subscription id
         */
        std::string id = Core::StringUtils::CreateRandomUuid();

        /**
         * AWS region
         */
        std::string region;

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
         * @brief Converts the DTO to a JSON string.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Subscription &r);
    };

    typedef std::vector<Subscription> SubscriptionsList;

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_SUBSCRIPTIONS_H
