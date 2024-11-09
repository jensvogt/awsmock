//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_LAMBDA_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_LAMBDA_CLIENT_COMMAND_H

// C++ includes
#include <sstream>
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/lexical_cast.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
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
        UNKNOWN
    };

    static std::map<LambdaCommandType, std::string> LambdaCommandTypeNames{
            {LambdaCommandType::CREATE_LAMBDA, "create-queue"},
            {LambdaCommandType::DELETE_LAMBDA, "delete-function"},
            {LambdaCommandType::INVOKE_LAMBDA, "invoke-function"},
            {LambdaCommandType::LIST_LAMBDAS, "list-functions"},
            {LambdaCommandType::LIST_LAMBDA_COUNTERS, "list-function-counters"},
            {LambdaCommandType::WAIT_LAMBDA, "wait"},
            {LambdaCommandType::CREATE_EVENT_SOURCE_MAPPING, "create-event-source-mapping"},
            {LambdaCommandType::LIST_EVENT_SOURCE_MAPPINGS, "list-event-source-mappings"},
            {LambdaCommandType::TAG_LAMBDA, "function-tag"}};

    [[maybe_unused]] static std::string LambdaCommandTypeToString(LambdaCommandType commandType) {
        return LambdaCommandTypeNames[commandType];
    }

    [[maybe_unused]] static LambdaCommandType LambdaCommandTypeFromString(const std::string &commandType) {
        for (auto &it: LambdaCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, it.second)) {
                return it.first;
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
    struct LambdaClientCommand : public BaseClientCommand {

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
         * @return DTO as string for logging.
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
