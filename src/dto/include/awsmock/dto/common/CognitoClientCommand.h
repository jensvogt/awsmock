//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_COGNITO_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_COGNITO_CLIENT_COMMAND_H

// C++ includes
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;

    /**
     * @brief Supported Cognito client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class CognitoCommandType {
        CREATE_USER_POOL,
        CREATE_USER_POOL_CLIENT,
        CREATE_USER_POOL_DOMAIN,
        UPDATE_USER_POOL,
        UPDATE_USER_POOL_DOMAIN,
        UPDATE_USER_POOL_CLIENT,
        LIST_USER_POOL_CLIENTS,
        LIST_USER_POOL_COUNTERS,
        DESCRIBE_USER_POOL,
        DESCRIBE_USER_POOL_CLIENTS,
        DELETE_USER_POOL,
        DELETE_USER_POOL_CLIENT,
        LIST_USER_POOLS,
        LIST_USERS,
        LIST_USER_COUNTERS,
        CREATE_USER,
        ENABLE_USER,
        DISABLE_USER,
        DELETE_USER,
        CREATE_GROUP,
        LIST_GROUPS,
        DELETE_GROUP,
        LIST_USERS_IN_GROUP,
        INITIATE_AUTH,
        RESPOND_TO_AUTH_CHALLENGE,
        SIGN_UP,
        GLOBAL_SIGN_OUT,
        ADMIN_CREATE_USER,
        ADMIN_GET_USER,
        ADMIN_ENABLE_USER,
        ADMIN_DISABLE_USER,
        ADMIN_DELETE_USER,
        ADMIN_ADD_USER_TO_GROUP,
        ADMIN_REMOVE_USER_FROM_GROUP,
        ADMIN_CONFIRM_SIGN_UP,
        UNKNOWN
    };

    static std::map<CognitoCommandType, std::string> CognitoCommandTypeNames{
            {CognitoCommandType::CREATE_USER_POOL, "create-user-pool"},
            {CognitoCommandType::CREATE_USER_POOL_DOMAIN, "create-user-pool-domain"},
            {CognitoCommandType::CREATE_USER_POOL_CLIENT, "create-user-pool-client"},
            {CognitoCommandType::UPDATE_USER_POOL, "update-user-pool"},
            {CognitoCommandType::UPDATE_USER_POOL_DOMAIN, "update-user-pool-domain"},
            {CognitoCommandType::UPDATE_USER_POOL_CLIENT, "update-user-pool-client"},
            {CognitoCommandType::DESCRIBE_USER_POOL, "describe-user-pool"},
            {CognitoCommandType::LIST_USER_POOLS, "list-user-pools"},
            {CognitoCommandType::LIST_USER_POOL_CLIENTS, "list-user-pool-clients"},
            {CognitoCommandType::LIST_USER_POOL_COUNTERS, "list-user-pool-counters"},
            {CognitoCommandType::LIST_USERS, "list-users"},
            {CognitoCommandType::LIST_USER_COUNTERS, "list-user-counters"},
            {CognitoCommandType::DESCRIBE_USER_POOL, "describe-user-pool"},
            {CognitoCommandType::DESCRIBE_USER_POOL_CLIENTS, "describe-user-pool-clients"},
            {CognitoCommandType::DELETE_USER_POOL, "delete-user-pool"},
            {CognitoCommandType::DELETE_USER_POOL_CLIENT, "delete-user-pool-client"},
            {CognitoCommandType::CREATE_USER, "create-user"},
            {CognitoCommandType::ENABLE_USER, "enable-user"},
            {CognitoCommandType::DISABLE_USER, "disable-user"},
            {CognitoCommandType::DELETE_USER, "delete-user"},
            {CognitoCommandType::CREATE_GROUP, "create-group"},
            {CognitoCommandType::LIST_GROUPS, "list-groups"},
            {CognitoCommandType::DELETE_GROUP, "delete-group"},
            {CognitoCommandType::LIST_USERS_IN_GROUP, "list-users-in-group"},
            {CognitoCommandType::INITIATE_AUTH, "initiate-auth"},
            {CognitoCommandType::RESPOND_TO_AUTH_CHALLENGE, "respond-to-auth-challenge"},
            {CognitoCommandType::SIGN_UP, "sign-up"},
            {CognitoCommandType::ADMIN_CREATE_USER, "admin-create-user"},
            {CognitoCommandType::ADMIN_GET_USER, "admin-get-user"},
            {CognitoCommandType::ADMIN_ENABLE_USER, "admin-enable-user"},
            {CognitoCommandType::ADMIN_DISABLE_USER, "admin-disable-user"},
            {CognitoCommandType::ADMIN_DELETE_USER, "admin-delete-user"},
            {CognitoCommandType::ADMIN_CONFIRM_SIGN_UP, "admin-confirm-sign-up"},
            {CognitoCommandType::GLOBAL_SIGN_OUT, "global-sign-out"},
            {CognitoCommandType::ADMIN_ADD_USER_TO_GROUP, "admin-add-user-to-group"},
            {CognitoCommandType::ADMIN_REMOVE_USER_FROM_GROUP, "admin-remove-user-from-group"}};

    [[maybe_unused]] static std::string CognitoCommandTypeToString(const CognitoCommandType commandType) {
        return CognitoCommandTypeNames[commandType];
    }

    [[maybe_unused]] static CognitoCommandType CognitoCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: CognitoCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return CognitoCommandType::UNKNOWN;
    }

    /**
     * @brief Cognito client command
     *
     * The Cognito client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a
     * different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CognitoClientCommand : BaseClientCommand {

        /**
         * Client command
         */
        CognitoCommandType command = CognitoCommandType::UNKNOWN;

        /**
         * Bucket
         */
        std::string poolName;

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
        friend std::ostream &operator<<(std::ostream &os, const CognitoClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_COGNITO_CLIENT_COMMAND_H
