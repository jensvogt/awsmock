//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_SSM_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_SSM_CLIENT_COMMAND_H

// C++ includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Supported System Manager client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class SSMCommandType {
        PUT_PARAMETER,
        GET_PARAMETER,
        DESCRIBE_PARAMETERS,
        DELETE_PARAMETER,
        UNKNOWN
    };

    static std::map<SSMCommandType, std::string> SSMCommandTypeNames{
            {SSMCommandType::PUT_PARAMETER, "put-parameter"},
            {SSMCommandType::GET_PARAMETER, "get-parameter"},
            {SSMCommandType::DESCRIBE_PARAMETERS, "describe-parameters"},
            {SSMCommandType::DELETE_PARAMETER, "delete-parameter"},
            {SSMCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string SSMCommandTypeToString(SSMCommandType commandType) {
        return SSMCommandTypeNames[commandType];
    }

    [[maybe_unused]] static SSMCommandType SSMCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: SSMCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return SSMCommandType::UNKNOWN;
    }

    /**
     * @brief ssm client command
     * <p>
     * The ssm client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a different way of
     * calling the AWS REST services.
     * </p>
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SSMClientCommand : BaseClientCommand {

        /**
         * Client command
         */
        SSMCommandType command{};

        /**
         * Returns the command from HTTP header
         *
         * @param request HTTP request
         * @return command string
         */
        static std::string GetCommandFromHeader(const http::request<http::dynamic_body> &request);

        /**
         * Gets the value from the user-agent string
         *
         * @param request HTTP server request
         * @param region AWS region
         * @param user AWS user
         */
        void FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

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
        friend std::ostream &operator<<(std::ostream &os, const SSMClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_SSM_CLIENT_COMMAND_H
