//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/sts/AssumeRoleRequest.h>
#include <awsmock/dto/sts/AssumeRoleResponse.h>
#include <awsmock/dto/sts/GetCallerIdentityResponse.h>

namespace Awsmock::Service {

    class STSService {

      public:

        explicit STSService() = default;

        Dto::STS::AssumeRoleResponse AssumeRole(const Dto::STS::AssumeRoleRequest &request);

        Dto::STS::GetCallerIdentityResponse GetCallerIdentity(const std::string &region);

      private:
        mutable logger_t _logger{boost::log::keywords::channel = "STS"};
    };

}// namespace Awsmock::Service
