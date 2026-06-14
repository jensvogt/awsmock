//
// Created by vogje01 on 11/26/23.
//

#pragma once

// C++ includes
#include <algorithm>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>

namespace Awsmock::Dto::Common {

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
        GET_REST_APIS,
        CREATE_REST_API,
        DELETE_REST_API,
        CREATE_RESOURCE,
        UPDATE_RESOURCE,
        DELETE_RESOURCE,
        PUT_METHOD,
        PUT_INTEGRATION,
        GET_RESOURCES,
        GET_RESOURCE,
        // AwsMock internal
        LIST_API_KEY_COUNTERS,
        GET_API_KEY_COUNTER,
        UPDATE_API_KEY_COUNTER,
        LIST_REST_API_COUNTERS,
        GET_REST_API_COUNTER,
        UPDATE_REST_API_COUNTER,
        DELETE_REST_API_COUNTER,
        UNKNOWN
    };

    static std::map<ApiGatewayCommandType, std::string> ApiGatewayCommandTypeNames{
            {ApiGatewayCommandType::CREATE_API_KEY, "create-api-key"},
            {ApiGatewayCommandType::GET_API_KEYS, "get-api-keys"},
            {ApiGatewayCommandType::DELETE_API_KEY, "delete-api-key"},
            {ApiGatewayCommandType::CREATE_REST_API, "create-rest-api"},
            {ApiGatewayCommandType::DELETE_REST_API, "delete-rest-api"},
            {ApiGatewayCommandType::CREATE_RESOURCE, "create-resource"},
            {ApiGatewayCommandType::GET_REST_APIS, "get-rest-apis"},
            {ApiGatewayCommandType::UPDATE_RESOURCE, "update-resource"},
            {ApiGatewayCommandType::DELETE_RESOURCE, "delete-resource"},
            {ApiGatewayCommandType::PUT_METHOD, "put-method"},
            {ApiGatewayCommandType::PUT_INTEGRATION, "put-integration"},
            {ApiGatewayCommandType::GET_RESOURCES, "get-resources"},
            {ApiGatewayCommandType::GET_RESOURCE, "get-resource"},
            // AwsMock internal commands
            {ApiGatewayCommandType::LIST_API_KEY_COUNTERS, "list-api-key-counters"},
            {ApiGatewayCommandType::GET_API_KEY_COUNTER, "get-api-key-counter"},
            {ApiGatewayCommandType::UPDATE_API_KEY_COUNTER, "update-api-key-counter"},
            {ApiGatewayCommandType::LIST_REST_API_COUNTERS, "list-rest-api-counters"},
            {ApiGatewayCommandType::GET_REST_API_COUNTER, "get-rest-api-counter"},
            {ApiGatewayCommandType::UPDATE_REST_API_COUNTER, "update-rest-api-counter"},
            {ApiGatewayCommandType::DELETE_REST_API_COUNTER, "delete-rest-api-counter"},
            {ApiGatewayCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string ApiGatewayCommandTypeToString(const ApiGatewayCommandType &commandType) {
        return ApiGatewayCommandTypeNames[commandType];
    }

    [[maybe_unused]] static ApiGatewayCommandType ApiGatewayCommandTypeFromString(const std::string &commandType) {
        const auto it = std::ranges::find_if(ApiGatewayCommandTypeNames, [&](const auto &p) { return p.second == commandType; });
        return it != ApiGatewayCommandTypeNames.end() ? it->first : ApiGatewayCommandType::UNKNOWN;
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

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Core"};
    };

}// namespace Awsmock::Dto::Common
