//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_APPLICATION_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_APPLICATION_CLIENT_COMMAND_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/AwsUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

    /**
     * @brief Supported application client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ApplicationCommandType {
        CREATE_APPLICATION,
        GET_APPLICATION,
        UPDATE_APPLICATION,
        UPLOAD_APPLICATION,
        LIST_APPLICATIONS,
        LIST_APPLICATION_NAMES,
        ENABLE_APPLICATION,
        ENABLE_ALL_APPLICATIONS,
        DISABLE_APPLICATION,
        DISABLE_ALL_APPLICATIONS,
        START_APPLICATION,
        START_ALL_APPLICATIONS,
        STOP_APPLICATION,
        RESTART_APPLICATION,
        RESTART_ALL_APPLICATIONS,
        REBUILD_APPLICATION,
        STOP_ALL_APPLICATIONS,
        DELETE_APPLICATION,
        UNKNOWN
    };

    static std::map<ApplicationCommandType, std::string> ApplicationCommandTypeNames{
            {ApplicationCommandType::CREATE_APPLICATION, "create-application"},
            {ApplicationCommandType::GET_APPLICATION, "get-application"},
            {ApplicationCommandType::UPDATE_APPLICATION, "update-application"},
            {ApplicationCommandType::UPLOAD_APPLICATION, "upload-application"},
            {ApplicationCommandType::LIST_APPLICATIONS, "list-applications"},
            {ApplicationCommandType::LIST_APPLICATION_NAMES, "list-application-names"},
            {ApplicationCommandType::ENABLE_APPLICATION, "enable-application"},
            {ApplicationCommandType::ENABLE_ALL_APPLICATIONS, "enable-all-applications"},
            {ApplicationCommandType::DISABLE_APPLICATION, "disable-application"},
            {ApplicationCommandType::DISABLE_ALL_APPLICATIONS, "disable-all-applications"},
            {ApplicationCommandType::START_APPLICATION, "start-application"},
            {ApplicationCommandType::START_ALL_APPLICATIONS, "start-all-applications"},
            {ApplicationCommandType::STOP_APPLICATION, "stop-application"},
            {ApplicationCommandType::STOP_ALL_APPLICATIONS, "stop-all-applications"},
            {ApplicationCommandType::RESTART_APPLICATION, "restart-application"},
            {ApplicationCommandType::RESTART_ALL_APPLICATIONS, "restart-all-applications"},
            {ApplicationCommandType::REBUILD_APPLICATION, "rebuild-application"},
            {ApplicationCommandType::DELETE_APPLICATION, "delete-application"},
            {ApplicationCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string ApplicationCommandTypeToString(const ApplicationCommandType &commandType) {
        return ApplicationCommandTypeNames[commandType];
    }

    [[maybe_unused]] static ApplicationCommandType ApplicationCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: ApplicationCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return ApplicationCommandType::UNKNOWN;
    }

    /**
     * @brief Application client command
     *
     * The Application client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a
     * different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ApplicationClientCommand : BaseClientCommand {

        /**
         * Client command
         */
        ApplicationCommandType command = ApplicationCommandType::UNKNOWN;

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
        friend std::ostream &operator<<(std::ostream &os, const ApplicationClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_APPLICATION_CLIENT_COMMAND_H
