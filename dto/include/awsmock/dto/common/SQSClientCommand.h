//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_SQS_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_SQS_CLIENT_COMMAND_H

// C++ includes
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/lexical_cast.hpp>

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
     * @brief Supported SQS client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class SqsCommandType {
        CREATE_QUEUE,
        PURGE_QUEUE,
        PURGE_ALL_QUEUES,
        GET_QUEUE_ATTRIBUTES,
        SET_QUEUE_ATTRIBUTES,
        GET_QUEUE_URL,
        GET_QUEUE_DETAILS,
        TAG_QUEUE,
        UNTAG_QUEUE,
        LIST_QUEUES,
        LIST_QUEUE_TAGS,
        LIST_QUEUE_ARNS,
        UPDATE_QUEUE,
        DELETE_QUEUE,
        SEND_MESSAGE,
        SEND_MESSAGE_BATCH,
        RECEIVE_MESSAGE,
        CHANGE_MESSAGE_VISIBILITY,
        LIST_MESSAGES,
        DELETE_MESSAGE,
        DELETE_MESSAGE_BATCH,
        // Awsmock
        LIST_QUEUE_COUNTERS,
        LIST_QUEUE_ATTRIBUTE_COUNTERS,
        LIST_LAMBDA_TRIGGER_COUNTERS,
        LIST_MESSAGE_COUNTERS,
        LIST_MESSAGE_ATTRIBUTE_COUNTERS,
        LIST_TAG_COUNTERS,
        GET_MESSAGE_COUNTERS,
        UPDATE_MESSAGE,
        REDRIVE_MESSAGE,
        REDRIVE_MESSAGES,
        ADD_ATTRIBUTE,
        LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS,
        ADD_DEFAULT_MESSAGE_ATTRIBUTE,
        UPDATE_DEFAULT_MESSAGE_ATTRIBUTE,
        DELETE_DEFAULT_MESSAGE_ATTRIBUTE,
        DELETE_ATTRIBUTE,
        UPDATE_DLQ,
        RESEND_MESSAGE,
        GET_EVENT_SOURCE,
        EXPORT_MESSAGES,
        IMPORT_MESSAGES,
        RELOAD_COUNTERS,
        RELOAD_ALL_COUNTERS,
        UNKNOWN
    };

    static std::map<SqsCommandType, std::string> SqsCommandTypeNames{
            // Queues
            {SqsCommandType::CREATE_QUEUE, "create-queue"},
            {SqsCommandType::PURGE_QUEUE, "purge-queue"},
            {SqsCommandType::PURGE_ALL_QUEUES, "purge-all-queues"},
            {SqsCommandType::GET_QUEUE_ATTRIBUTES, "get-queue-attributes"},
            {SqsCommandType::SET_QUEUE_ATTRIBUTES, "set-queue-attributes"},
            {SqsCommandType::GET_QUEUE_URL, "get-queue-url"},
            {SqsCommandType::GET_QUEUE_DETAILS, "get-queue-details"},
            {SqsCommandType::TAG_QUEUE, "tag-queue"},
            {SqsCommandType::UNTAG_QUEUE, "untag-queue"},
            {SqsCommandType::LIST_QUEUES, "list-queues"},
            {SqsCommandType::LIST_QUEUE_TAGS, "list-queue-tags"},
            {SqsCommandType::LIST_QUEUE_ARNS, "list-queue-arns"},
            {SqsCommandType::UPDATE_QUEUE, "update-queue"},
            {SqsCommandType::DELETE_QUEUE, "delete-queue"},
            // Messages
            {SqsCommandType::SEND_MESSAGE, "send-message"},
            {SqsCommandType::SEND_MESSAGE_BATCH, "send-message-batch"},
            {SqsCommandType::RECEIVE_MESSAGE, "receive-message"},
            {SqsCommandType::CHANGE_MESSAGE_VISIBILITY, "change-message-visibility"},
            {SqsCommandType::LIST_MESSAGES, "list-messages"},
            {SqsCommandType::DELETE_MESSAGE, "delete-message"},
            {SqsCommandType::DELETE_MESSAGE_BATCH, "delete-message-batch"},
            // Awsmock
            {SqsCommandType::LIST_QUEUE_COUNTERS, "list-queue-counters"},
            {SqsCommandType::LIST_TAG_COUNTERS, "list-tag-counters"},
            {SqsCommandType::LIST_QUEUE_ATTRIBUTE_COUNTERS, "list-queue-attribute-counters"},
            {SqsCommandType::LIST_LAMBDA_TRIGGER_COUNTERS, "list-lambda-trigger-counters"},
            {SqsCommandType::LIST_MESSAGE_COUNTERS, "list-message-counters"},
            {SqsCommandType::LIST_MESSAGE_ATTRIBUTE_COUNTERS, "list-message-attribute-counters"},
            {SqsCommandType::GET_MESSAGE_COUNTERS, "get-message-counters"},
            {SqsCommandType::UPDATE_MESSAGE, "update-message"},
            {SqsCommandType::UPDATE_DLQ, "update-dlq"},
            {SqsCommandType::REDRIVE_MESSAGE, "redrive-message"},
            {SqsCommandType::REDRIVE_MESSAGES, "redrive-messages"},
            {SqsCommandType::RESEND_MESSAGE, "resend-message"},
            {SqsCommandType::GET_EVENT_SOURCE, "get-event-source"},
            {SqsCommandType::ADD_ATTRIBUTE, "add-attribute"},
            {SqsCommandType::LIST_DEFAULT_MESSAGE_ATTRIBUTE_COUNTERS, "list-default-message-attribute-counters"},
            {SqsCommandType::ADD_DEFAULT_MESSAGE_ATTRIBUTE, "add-default-message-attribute-counter"},
            {SqsCommandType::UPDATE_DEFAULT_MESSAGE_ATTRIBUTE, "update-default-message-attribute-counter"},
            {SqsCommandType::DELETE_DEFAULT_MESSAGE_ATTRIBUTE, "delete-default-message-attribute-counter"},
            {SqsCommandType::DELETE_ATTRIBUTE, "delete-attribute"},
            {SqsCommandType::DELETE_MESSAGE_BATCH, "delete-message-batch"},
            {SqsCommandType::EXPORT_MESSAGES, "export-messages"},
            {SqsCommandType::IMPORT_MESSAGES, "import-messages"},
            {SqsCommandType::RELOAD_COUNTERS, "reload-counters"},
            {SqsCommandType::RELOAD_ALL_COUNTERS, "reload-all-counters"},
    };

    [[maybe_unused]] static std::string SqsCommandTypeToString(const SqsCommandType &commandType) {
        return SqsCommandTypeNames[commandType];
    }

    [[maybe_unused]] static SqsCommandType SqsCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: SqsCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return SqsCommandType::UNKNOWN;
    }

    /**
     * @brief S3 client command is used as a standardized way of interpreting the different ways the clients are calling the REST services.
     *
     * Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SQSClientCommand : BaseClientCommand {

        /**
         * @brief Client command
         */
        SqsCommandType command = SqsCommandType::UNKNOWN;

        /**
         * @brief Returns the command from HTTP header
         *
         * @param request HTTP request
         * @return command string
         */
        [[nodiscard]] std::string GetCommandFromHeader(const http::request<http::dynamic_body> &request) const;

        /**
         * @brief Gets the value from the user-agent string
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
        friend std::ostream &operator<<(std::ostream &os, const SQSClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_SQS_CLIENT_COMMAND_H
