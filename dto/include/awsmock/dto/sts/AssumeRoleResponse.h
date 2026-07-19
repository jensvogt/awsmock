//
// Created by vogje01 on 19/07/2026.
//

#pragma once

#include <string>

#include <awsmock/core/AwsUtils.h>

namespace Awsmock::Dto::STS {

    struct AssumeRoleResponse {

        std::string requestId;
        std::string roleArn;
        std::string roleSessionName;

        // Fake static credentials — sufficient for local awsmock testing
        std::string accessKeyId = "ASIAIOSFODNN7EXAMPLE";
        std::string secretAccessKey = "wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY";
        std::string sessionToken = "AQoXnyc4lcK4w4OIaHgBQIGPaFakeSessionTokenForAwsMock";
        std::string expiration = "2099-01-01T00:00:00Z";

        [[nodiscard]] std::string ToXml() const {

            // Derive assumed-role ARN from the role ARN, e.g.
            // arn:aws:iam::123:role/gitlab-ci  ->  arn:aws:sts::123:assumed-role/gitlab-ci/<session>
            std::string assumedRoleArn = roleArn;
            const auto pos = assumedRoleArn.find(":role/");
            if (pos != std::string::npos) {
                assumedRoleArn.replace(pos, 6, ":assumed-role/");
                assumedRoleArn += "/" + roleSessionName;
            }

            std::ostringstream oss;
            oss << R"(<?xml version="1.0" encoding="UTF-8"?>)"
                << R"(<AssumeRoleResponse xmlns="https://sts.amazonaws.com/doc/2011-06-15/">)"
                << "<AssumeRoleResult>"
                << "<Credentials>"
                << "<AccessKeyId>" << accessKeyId << "</AccessKeyId>"
                << "<SecretAccessKey>" << secretAccessKey << "</SecretAccessKey>"
                << "<SessionToken>" << sessionToken << "</SessionToken>"
                << "<Expiration>" << expiration << "</Expiration>"
                << "</Credentials>"
                << "<AssumedRoleUser>"
                << "<Arn>" << assumedRoleArn << "</Arn>"
                << "<AssumedRoleId>AROAIOSFODNN7EXAMPLE:" << roleSessionName << "</AssumedRoleId>"
                << "</AssumedRoleUser>"
                << "</AssumeRoleResult>"
                << "<ResponseMetadata><RequestId>" << requestId << "</RequestId></ResponseMetadata>"
                << "</AssumeRoleResponse>";
            return oss.str();
        }
    };

}// namespace Awsmock::Dto::STS
