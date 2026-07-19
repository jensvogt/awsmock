//
// Created by vogje01 on 19/07/2026.
//

#include <awsmock/service/sts/STSService.h>

namespace Awsmock::Service {

    Dto::STS::AssumeRoleResponse STSService::AssumeRole(const Dto::STS::AssumeRoleRequest &request) {
        log_debug << "AssumeRole, roleArn: " << request.roleArn;

        Dto::STS::AssumeRoleResponse response;
        response.requestId = Core::AwsUtils::CreateRequestId();
        response.roleArn = request.roleArn;
        response.roleSessionName = request.roleSessionName;
        return response;
    }

    Dto::STS::GetCallerIdentityResponse STSService::GetCallerIdentity(const std::string &region) {
        log_debug << "GetCallerIdentity, region: " << region;

        Dto::STS::GetCallerIdentityResponse response;
        response.requestId = Core::AwsUtils::CreateRequestId();
        return response;
    }

}// namespace Awsmock::Service
