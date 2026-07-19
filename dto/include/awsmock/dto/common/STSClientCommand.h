//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/common/BaseClientCommand.h>
#include <awsmock/dto/common/UserAgent.h>

namespace Awsmock::Dto::Common {

    namespace http = boost::beast::http;

    enum class STSCommandType {
        ASSUME_ROLE,
        GET_CALLER_IDENTITY,
        UNKNOWN
    };

    static std::map<STSCommandType, std::string> STSCommandTypeNames{
            {STSCommandType::ASSUME_ROLE, "assume-role"},
            {STSCommandType::GET_CALLER_IDENTITY, "get-caller-identity"},
            {STSCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string STSCommandTypeToString(STSCommandType commandType) {
        return STSCommandTypeNames[commandType];
    }

    [[maybe_unused]] static STSCommandType STSCommandTypeFromString(const std::string &commandType) {
        for (auto &[fst, snd]: STSCommandTypeNames) {
            if (Core::StringUtils::EqualsIgnoreCase(commandType, snd)) {
                return fst;
            }
        }
        return STSCommandType::UNKNOWN;
    }

    struct STSClientCommand : BaseClientCommand {

        STSCommandType command = STSCommandType::UNKNOWN;

        void FromRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user);

        [[nodiscard]] std::string ToJson() const;
        [[nodiscard]] std::string ToString() const;
        friend std::ostream &operator<<(std::ostream &os, const STSClientCommand &i);

      private:
        mutable logger_t _logger{boost::log::keywords::channel = "Core"};
    };

}// namespace Awsmock::Dto::Common
