//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_SNS_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_SNS_CLIENT_COMMAND_H

// C++ includes
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {
    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Supported SNS client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class SNSCommandType {
        CREATE_TOPIC,
        LIST_TOPICS,
        GET_TOPIC_ATTRIBUTES,
        GET_TOPIC_DETAILS,
        PURGE_TOPIC,
        PURGE_ALL_TOPICS,
        PUBLISH,
        SUBSCRIBE,
        UNSUBSCRIBE,
        LIST_SUBSCRIPTIONS_BY_TOPIC,
        TAG_RESOURCE,
        UNTAG_RESOURCE,
        DELETE_TOPIC,
        LIST_MESSAGES,
        DELETE_MESSAGE,
        // AwsMock
        LIST_TOPIC_COUNTERS,
        LIST_SUBSCRIPTION_COUNTERS,
        LIST_TAG_COUNTERS,
        LIST_ATTRIBUTE_COUNTERS,
        LIST_MESSAGE_COUNTERS,
        LIST_TOPIC_ARNS,
        UPDATE_SUBSCRIPTION,
        GET_EVENT_SOURCE,
        GET_MESSAGE_COUNTERS,
        PUBLISH_COUNTER,
        UNKNOWN
    };

    static std::map<SNSCommandType, std::string> SNSCommandTypeNames{
        {SNSCommandType::CREATE_TOPIC, "create-topic"},
        {SNSCommandType::LIST_TOPICS, "list-topics"},
        {SNSCommandType::GET_TOPIC_ATTRIBUTES, "get-topic-attributes"},
        {SNSCommandType::GET_TOPIC_DETAILS, "get-topic-details"},
        {SNSCommandType::PURGE_TOPIC, "purge-topic"},
        {SNSCommandType::PURGE_ALL_TOPICS, "purge-all-topics"},
        {SNSCommandType::PUBLISH, "publish"},
        {SNSCommandType::SUBSCRIBE, "subscribe"},
        {SNSCommandType::UNSUBSCRIBE, "unsubscribe"},
        {SNSCommandType::LIST_SUBSCRIPTIONS_BY_TOPIC, "list-subscriptions-by-topic"},
        {SNSCommandType::TAG_RESOURCE, "tag-resource"},
        {SNSCommandType::UNTAG_RESOURCE, "untag-resource"},
        {SNSCommandType::DELETE_TOPIC, "delete-topic"},
        {SNSCommandType::LIST_MESSAGES, "list-messages"},
        {SNSCommandType::DELETE_MESSAGE, "delete-message"},
        // AwsMock
        {SNSCommandType::LIST_TOPIC_COUNTERS, "list-topic-counters"},
        {SNSCommandType::LIST_SUBSCRIPTION_COUNTERS, "list-subscription-counters"},
        {SNSCommandType::LIST_TAG_COUNTERS, "list-tag-counters"},
        {SNSCommandType::LIST_ATTRIBUTE_COUNTERS, "list-attribute-counters"},
        {SNSCommandType::UPDATE_SUBSCRIPTION, "update-subscription"},
        {SNSCommandType::LIST_MESSAGE_COUNTERS, "list-message-counters"},
        {SNSCommandType::LIST_TOPIC_ARNS, "list-topic-arns"},
        {SNSCommandType::GET_EVENT_SOURCE, "get-event-source"},
        {SNSCommandType::GET_MESSAGE_COUNTERS, "get-message-counters"},
        {SNSCommandType::PUBLISH_COUNTER, "publish-counter"},
    };

    [[maybe_unused]] static std::string SNSCommandTypeToString(SNSCommandType commandType) {
        return SNSCommandTypeNames[commandType];
    }

    [[maybe_unused]] static SNSCommandType SNSCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: SNSCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return SNSCommandType::UNKNOWN;
    }

    /**
     * @brief The SNS client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SNSClientCommand : BaseClientCommand {
        /**
         * @brief Client command
         */
        SNSCommandType command = SNSCommandType::UNKNOWN;

        /**
         * @brief Returns the command from the HTTP header
         *
         * @param request HTTP request
         * @return command string
         */
        [[nodiscard]] std::string GetCommandFromHeader(const http::request<http::dynamic_body> &request) const;

        /**
         * @brief Getś the value from the user-agent string
         *
         * @param request HTTP server request
         * @param region AWS region
         * @param user AWS user
         */
        void FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const SNSClientCommand &i);
    };
} // namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_SNS_CLIENT_COMMAND_H
