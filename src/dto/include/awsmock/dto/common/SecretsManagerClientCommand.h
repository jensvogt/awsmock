//
// Created by vogje01 on 07/04/2024.
//

#ifndef AWSMOCK_DTO_COMMON_SECRETSMANAGER_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_SECRETSMANAGER_CLIENT_COMMAND_H

// C++ includes
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

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
     * @brief Supported secrets manager client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class SecretsManagerCommandType {
        CREATE_SECRET,
        DELETE_SECRET,
        DESCRIBE_SECRET,
        GET_SECRET_VALUE,
        UPDATE_SECRET,
        ROTATE_SECRET,
        LIST_SECRETS,
        UNKNOWN
    };

    static std::map<SecretsManagerCommandType, std::string> SecretsManagerCommandTypeNames{
            {SecretsManagerCommandType::CREATE_SECRET, "CreateSecret"},
            {SecretsManagerCommandType::DELETE_SECRET, "DeleteSecret"},
            {SecretsManagerCommandType::DESCRIBE_SECRET, "DescribeSecret"},
            {SecretsManagerCommandType::GET_SECRET_VALUE, "GetSecretValue"},
            {SecretsManagerCommandType::UPDATE_SECRET, "UpdateSecret"},
            {SecretsManagerCommandType::ROTATE_SECRET, "RotateSecret"},
            {SecretsManagerCommandType::LIST_SECRETS, "ListSecrets"},
            {SecretsManagerCommandType::UNKNOWN, "Unknown"},
    };

    [[maybe_unused]] static std::string SecretsManagerCommandTypeToString(SecretsManagerCommandType commandType) {
        return SecretsManagerCommandTypeNames[commandType];
    }

    [[maybe_unused]] static SecretsManagerCommandType SecretsManagerCommandTypeFromString(const std::string &commandType) {
        for (auto &it: SecretsManagerCommandTypeNames) {
            if (Core::StringUtils::StartsWith(commandType, it.second)) {
                return it.first;
            }
        }
        return SecretsManagerCommandType::UNKNOWN;
    }

    /**
     * @brief The SecretsManager client command is used as a standardized way of interpreting the different ways the clients are calling the REST services.
     *
     * Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SecretsManagerClientCommand : BaseClientCommand {

        /**
         * @brief Client command
         */
        SecretsManagerCommandType command{};

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
        friend std::ostream &operator<<(std::ostream &os, const SecretsManagerClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_SECRETSMANAGER_CLIENT_COMMAND_H
