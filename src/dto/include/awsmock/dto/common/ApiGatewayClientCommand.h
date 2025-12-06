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
        GET_API_KEYS,
        DELETE_API_KEY,
        CREATE_REST_API,
        // AwsMock internal
        LIST_API_KEY_COUNTERS,
        GET_API_KEY_COUNTER,
        UPDATE_API_KEY_COUNTER,
        LIST_REST_API_COUNTERS,
        UNKNOWN
    };

    static std::map<ApiGatewayCommandType, std::string> ApiGatewayCommandTypeNames{
            {ApiGatewayCommandType::CREATE_API_KEY, "create-api-key"},
            {ApiGatewayCommandType::GET_API_KEYS, "get-api-keys"},
            {ApiGatewayCommandType::DELETE_API_KEY, "delete-api-key"},
            {ApiGatewayCommandType::CREATE_REST_API, "create-rest-api"},
            // AwsMock internal commands
            {ApiGatewayCommandType::LIST_API_KEY_COUNTERS, "list-api-key-counters"},
            {ApiGatewayCommandType::GET_API_KEY_COUNTER, "get-api-key-counter"},
            {ApiGatewayCommandType::UPDATE_API_KEY_COUNTER, "update-api-key-counter"},
            {ApiGatewayCommandType::LIST_REST_API_COUNTERS, "list-rest-api-counters"},
            {ApiGatewayCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string ApiGatewayCommandTypeToString(const ApiGatewayCommandType commandType) {
        return ApiGatewayCommandTypeNames[commandType];
    }

    [[maybe_unused]] static ApiGatewayCommandType ApiGatewayCommandTypeFromString(const std::string &commandType) {
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
        ApiGatewayCommandType command{};

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
