//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_CONTAINER_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_CONTAINER_CLIENT_COMMAND_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {
    /**
     * @brief Supported container client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ContainerCommandType {
        LIST_CONTAINERS,
        START_CONTAINER,
        STOP_CONTAINER,
        KILL_CONTAINER,
        LIST_CONTAINER_STATS,
        DELETE_CONTAINER,
        UNKNOWN
    };

    static std::map<ContainerCommandType, std::string> ContainerCommandTypeNames{
        {ContainerCommandType::LIST_CONTAINERS, "list-containers"},
        {ContainerCommandType::START_CONTAINER, "start-container"},
        {ContainerCommandType::STOP_CONTAINER, "stop-container"},
        {ContainerCommandType::KILL_CONTAINER, "kill-container"},
        {ContainerCommandType::LIST_CONTAINER_STATS, "list-container-stats"},
        {ContainerCommandType::DELETE_CONTAINER, "delete-container"},
        {ContainerCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string ContainerCommandTypeToString(const ContainerCommandType &commandType) {
        return ContainerCommandTypeNames[commandType];
    }

    [[maybe_unused]] static ContainerCommandType ContainerCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: ContainerCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return ContainerCommandType::UNKNOWN;
    }

    /**
     * @brief Application client command
     *
     * The Application client command is used as a standardized way of interpreting the different ways the clients are calling the REST services. Each client type is using a
     * different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ContainerClientCommand : BaseClientCommand {
        /**
         * Client command
         */
        ContainerCommandType command = ContainerCommandType::UNKNOWN;

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
        friend std::ostream &operator<<(std::ostream &os, const ContainerClientCommand &i);
    };
} // namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_CONTAINER_CLIENT_COMMAND_H
