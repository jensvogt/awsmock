//
// Created by vogje01 on 01/06/2023.
//

#pragma once

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/sns/MessageAttribute.h>
#include <awsmock/entity/sns/Subscription.h>
#include <awsmock/entity/sns/TopicAttribute.h>
#include <awsmock/utils/MongoUtils.h>

namespace Awsmock::Database::Entity::SNS {

    /**
     * @brief SNS topic entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Topic {

        /**
         * ID
         */
        std::string oid;

        /**
         * AWS region
         */
        std::string region;

        /**
         * Topic name
         */
        std::string topicName;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Topic URL
         */
        std::string topicUrl;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Target ARN
         */
        std::string targetArn;

        /**
         * Subscriptions
         */
        SubscriptionList subscriptions;

        /**
         * Attributes
         */
        TopicAttribute topicAttribute;

        /**
         * Topic tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Default message attributes
         */
        std::map<std::string, MessageAttribute> defaultMessageAttributes;

        /**
         * Total size of all messages in bytes
         */
        std::int64_t size{};

        /**
         * Total number of all messages
         */
        std::int64_t messages{};

        /**
         * Total number of all messages send
         */
        std::int64_t messagesSend{};

        /**
         * Total number of all messages resend
         */
        std::int64_t messagesResend{};

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified;

        /**
         * @brief Checks whether a subscription with the given protocol/endpoint exists already.
         *
         * @param subscription subscription object
         * @return true if a subscription exists.
         */
        bool HasSubscription(const Subscription &subscription);

        /**
         * @brief Checks whether a subscription with the given ARN exists already.
         *
         * @param subscriptionArn subscription ARN
         * @return true if a subscription with the given ARN exists.
         */
        bool HasSubscription(const std::string &subscriptionArn);

        /**
         * @brief Return the index of a subscription with the given ARN.
         *
         * @param subscriptionArn subscription ARN
         * @return subscription index
         */
        int GetSubscriptionIndex(const std::string &subscriptionArn);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        void FromDocument(const view_or_value<view, value> &mResult);

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "SNS"};
    };

    typedef std::vector<Topic> TopicList;

}// namespace Awsmock::Database::Entity::SNS
