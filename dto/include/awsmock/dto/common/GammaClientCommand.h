//
// Created by vogje01 on 11/26/23.
//

#pragma once

// C++ includes
#include <string>

// Boost includes
#include <boost/beast/http/message.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace Awsmock::Dto::Common {

    /**
     * @brief Supported Gamma client commands
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class GammaCommandType {
        GAMMA_RUNTIME_STATUS,
        UNKNOWN
    };

    static std::map<GammaCommandType, std::string> GammaCommandTypeNames{
            {GammaCommandType::GAMMA_RUNTIME_STATUS, "gamma-runtime-status"},
    };

    [[maybe_unused]] static std::string GammaCommandTypeToString(const GammaCommandType &commandType) {
        return GammaCommandTypeNames[commandType];
    }

    [[maybe_unused]] static GammaCommandType GammaCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: GammaCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return GammaCommandType::UNKNOWN;
    }

    /**
     * @brief Gamma client command is used as a standardized way of interpreting the different ways the clients are calling the REST services.
     *
     * Each client type is using a different way of calling the AWS REST services.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GammaClientCommand : BaseClientCommand {

        /**
         * @brief Client command
         */
        GammaCommandType command{};

        /**
         * @brief Returns the command from the HTTP header
         *
         * @param request HTTP request
         * @return command string
         */
        [[nodiscard]] std::string GetCommandFromHeader(const http::request<http::dynamic_body> &request) const;

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

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Core"};
    };
}// namespace Awsmock::Dto::Common
