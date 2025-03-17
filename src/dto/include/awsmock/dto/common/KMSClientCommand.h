//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_KMS_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_KMS_CLIENT_COMMAND_H

// C++ includes
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
     * @brief Supported KMS client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class KMSCommandType {
        CREATE_KEY,
        SCHEDULE_KEY_DELETION,
        DESCRIBE_KEY,
        LIST_KEYS,
        ENCRYPT,
        DECRYPT,
        UNKNOWN
    };

    static std::map<KMSCommandType, std::string> KMSCommandTypeNames{
            {KMSCommandType::CREATE_KEY, "create-key"},
            {KMSCommandType::SCHEDULE_KEY_DELETION, "schedule-key-deletion"},
            {KMSCommandType::DESCRIBE_KEY, "describe-key"},
            {KMSCommandType::ENCRYPT, "encrypt"},
            {KMSCommandType::DECRYPT, "decrypt"},
            {KMSCommandType::LIST_KEYS, "list-keys"},
    };

    [[maybe_unused]] static std::string KMSCommandTypeToString(KMSCommandType commandType) {
        return KMSCommandTypeNames[commandType];
    }

    [[maybe_unused]] static KMSCommandType KMSCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: KMSCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return KMSCommandType::UNKNOWN;
    }

    /**
     * KMS client command
     * <p>
     * The KMS client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a different way of
     * calling the AWS REST services.
     * </p>
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct KMSClientCommand : public BaseClientCommand {

        /**
         * Client command
         */
        KMSCommandType command{};

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
        friend std::ostream &operator<<(std::ostream &os, const KMSClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_KMS_CLIENT_COMMAND_H
