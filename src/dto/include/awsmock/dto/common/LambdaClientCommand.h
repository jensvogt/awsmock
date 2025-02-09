//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_LAMBDA_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_LAMBDA_CLIENT_COMMAND_H

// C++ includes
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    enum class LambdaCommandType {
        CREATE_LAMBDA,
        DELETE_LAMBDA,
        INVOKE_LAMBDA,
        LIST_LAMBDAS,
        LIST_LAMBDA_COUNTERS,
        WAIT_LAMBDA,
        CREATE_EVENT_SOURCE_MAPPING,
        LIST_EVENT_SOURCE_MAPPINGS,
        TAG_LAMBDA,
        GET_FUNCTION_COUNTERS,
        RESET_FUNCTION_COUNTERS,
        UPLOAD_FUNCTION_CODE,
        LIST_TAG_COUNTERS,
        ADD_TAG,
        DELETE_TAG,
        UPDATE_TAG,
        LIST_ENVIRONMENT_COUNTERS,
        ADD_ENVIRONMENT,
        DELETE_ENVIRONMENT,
        UPDATE_ENVIRONMENT,
        START_FUNCTION,
        STOP_FUNCTION,
        DELETE_IMAGE,
        UNKNOWN
    };

    static std::map<LambdaCommandType, std::string> LambdaCommandTypeNames{
            {LambdaCommandType::CREATE_LAMBDA, "create-function"},
            {LambdaCommandType::DELETE_LAMBDA, "delete-function"},
            {LambdaCommandType::INVOKE_LAMBDA, "invoke-function"},
            {LambdaCommandType::LIST_LAMBDAS, "list-functions"},
            {LambdaCommandType::LIST_LAMBDA_COUNTERS, "list-function-counters"},
            {LambdaCommandType::WAIT_LAMBDA, "wait"},
            {LambdaCommandType::CREATE_EVENT_SOURCE_MAPPING, "create-event-source-mapping"},
            {LambdaCommandType::LIST_EVENT_SOURCE_MAPPINGS, "list-event-source-mappings"},
            {LambdaCommandType::TAG_LAMBDA, "function-tag"},
            {LambdaCommandType::GET_FUNCTION_COUNTERS, "get-function-counters"},
            {LambdaCommandType::RESET_FUNCTION_COUNTERS, "reset-function-counters"},
            {LambdaCommandType::UPLOAD_FUNCTION_CODE, "upload-function-code"},
            {LambdaCommandType::LIST_TAG_COUNTERS, "list-tag-counters"},
            {LambdaCommandType::ADD_TAG, "add-function-tag"},
            {LambdaCommandType::DELETE_TAG, "delete-function-tag"},
            {LambdaCommandType::UPDATE_TAG, "update-function-tag"},
            {LambdaCommandType::LIST_ENVIRONMENT_COUNTERS, "list-environment-counters"},
            {LambdaCommandType::ADD_ENVIRONMENT, "add-function-environment"},
            {LambdaCommandType::DELETE_ENVIRONMENT, "delete-function-environment"},
            {LambdaCommandType::UPDATE_ENVIRONMENT, "update-function-environment"},
            {LambdaCommandType::START_FUNCTION, "start-function"},
            {LambdaCommandType::STOP_FUNCTION, "stop-function"},
            {LambdaCommandType::DELETE_IMAGE, "delete-image"},
    };

    [[maybe_unused]] static std::string LambdaCommandTypeToString(const LambdaCommandType &commandType) {
        return LambdaCommandTypeNames[commandType];
    }

    [[maybe_unused]] static LambdaCommandType LambdaCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: LambdaCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return LambdaCommandType::UNKNOWN;
    }

    /**
     * @brief S3 client command is used as a standardized way of interpreting the different ways the clients are calling the REST services.
     *
     * Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LambdaClientCommand : BaseClientCommand {

        /**
         * @brief Client command
         */
        LambdaCommandType command{};

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
        friend std::ostream &operator<<(std::ostream &os, const LambdaClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_LAMBDA_CLIENT_COMMAND_H
