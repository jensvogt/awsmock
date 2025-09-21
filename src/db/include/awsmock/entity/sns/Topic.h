//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SNS_TOPIC_H
#define AWSMOCK_DB_ENTITY_SNS_TOPIC_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/sns/Subscription.h>
#include <awsmock/entity/sns/TopicAttribute.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::SNS {

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
         * Total size of all messages in bytes
         */
        long size{};

        /**
         * Total number of all messages
         */
        long messages{};

        /**
         * Total number of all messages send
         */
        long messagesSend{};

        /**
         * Total number of all messages resend
         */
        long messagesResend{};

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

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Topic &q);
    };

    typedef std::vector<Topic> TopicList;

}// namespace AwsMock::Database::Entity::SNS

#endif// AWSMOCK_DB_ENTITY_SNS_TOPIC_H
