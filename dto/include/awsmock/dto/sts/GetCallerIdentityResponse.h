//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

namespace Awsmock::Dto::STS {

    struct GetCallerIdentityResponse {

        std::string requestId;
        std::string accountId = "000000000000";
        std::string userId = "000000000000";
        std::string arn = "arn:aws:iam::000000000000:root";

        [[nodiscard]] std::string ToXml() const {
            std::ostringstream oss;
            oss << R"(<?xml version="1.0" encoding="UTF-8"?>)"
                << R"(<GetCallerIdentityResponse xmlns="https://sts.amazonaws.com/doc/2011-06-15/">)"
                << "<GetCallerIdentityResult>"
                << "<Arn>" << arn << "</Arn>"
                << "<UserId>" << userId << "</UserId>"
                << "<Account>" << accountId << "</Account>"
                << "</GetCallerIdentityResult>"
                << "<ResponseMetadata><RequestId>" << requestId << "</RequestId></ResponseMetadata>"
                << "</GetCallerIdentityResponse>";
            return oss.str();
        }
    };

}// namespace Awsmock::Dto::STS
