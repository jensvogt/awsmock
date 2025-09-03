//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_API_GATEWAY_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_API_GATEWAY_CLIENT_COMMAND_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;

    /**
     * @brief Supported application client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ApiGatewayCommandType {
        CREATE_API_KEY,
        UNKNOWN
    };

    static std::map<ApiGatewayCommandType, std::string> ApiGatewayCommandTypeNames{
            {ApiGatewayCommandType::CREATE_API_KEY, "create-api-key"},
            {ApiGatewayCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string ApplicationCommandTypeToString(const ApiGatewayCommandType commandType) {
        return ApiGatewayCommandTypeNames[commandType];
    }

    [[maybe_unused]] static ApiGatewayCommandType ApplicationCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: ApiGatewayCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return ApiGatewayCommandType::UNKNOWN;
    }

    /**
     * @brief API gateway client command
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ApiGatewayClientCommand : BaseClientCommand {

        /**
         * Client command
         */
        ApiGatewayCommandType command;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Gets the value from the user-agent string
         *
         * @param request HTTP server request
         * @param region AWS region
         * @param user AWS user
         */
        void FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

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
        friend std::ostream &operator<<(std::ostream &os, const ApiGatewayClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_API_GATEWAY_CLIENT_COMMAND_H
