//
// Created by vogje01 on 11/26/23.
//

#ifndef AWSMOCK_DTO_COMMON_MONITORING_CLIENT_COMMAND_H
#define AWSMOCK_DTO_COMMON_MONITORING_CLIENT_COMMAND_H

// C++ includes
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/lexical_cast.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace AwsMock::Dto::Common {

    namespace http = boost::beast::http;
    namespace ip = boost::asio::ip;

    /**
     * @brief Supported monitoring client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class MonitoringCommandType {
        GET_COUNTERS,
        GET_MULTI_COUNTERS,
        UNKNOWN
    };

    static std::map<MonitoringCommandType, std::string> MonitoringCommandTypeNames{
            {MonitoringCommandType::GET_COUNTERS, "get-counters"},
            {MonitoringCommandType::GET_MULTI_COUNTERS, "get-multi-counters"}};

    [[maybe_unused]] static std::string MonitoringCommandTypeToString(MonitoringCommandType commandType) {
        return MonitoringCommandTypeNames[commandType];
    }

    [[maybe_unused]] static MonitoringCommandType MonitoringCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: MonitoringCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return MonitoringCommandType::UNKNOWN;
    }

    /**
     * @brief S3 client command is used as a standardized way of interpreting the different ways the clients are calling the REST services.
     *
     * Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MonitoringClientCommand : public BaseClientCommand {

        /**
         * @brief Client command
         */
        MonitoringCommandType command{};

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
        friend std::ostream &operator<<(std::ostream &os, const MonitoringClientCommand &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_MONITORING_CLIENT_COMMAND_H
