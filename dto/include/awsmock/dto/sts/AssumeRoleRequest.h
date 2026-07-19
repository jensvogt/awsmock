//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/common/STSClientCommand.h>

namespace Awsmock::Dto::STS {

    struct AssumeRoleRequest {

        std::string region;
        std::string roleArn;
        std::string roleSessionName;
        int durationSeconds = 3600;

        static AssumeRoleRequest FromClientCommand(const Dto::Common::STSClientCommand &command) {
            AssumeRoleRequest request;
            request.region = command.region;
            request.roleArn = Core::HttpUtils::GetStringParameterFromBody(command.payload, "RoleArn");
            request.roleSessionName = Core::HttpUtils::GetStringParameterFromBody(command.payload, "RoleSessionName");
            const std::string dur = Core::HttpUtils::GetStringParameterFromBody(command.payload, "DurationSeconds");
            if (!dur.empty()) {
                request.durationSeconds = std::stoi(dur);
            }
            return request;
        }
    };

}// namespace Awsmock::Dto::STS
