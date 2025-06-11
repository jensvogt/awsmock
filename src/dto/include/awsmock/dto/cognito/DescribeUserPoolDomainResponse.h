//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_DOMAIN_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_DOMAIN_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Describe user pool domain response
     *
     * Example:
     * @code{.json}
     * {
     *   "DomainDescription": {
     *      "AWSAccountId": "string",
     *      "CloudFrontDistribution": "string",
     *      "CustomDomainConfig": {
     *         "CertificateArn": "string"
     *      },
     *      "Domain": "string",
     *      "S3Bucket": "string",
     *      "Status": "string",
     *      "UserPoolId": "string",
     *      "Version": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeUserPoolDomainResponse final : Common::BaseCounter<DescribeUserPoolDomainResponse> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Domain
         */
        std::string domain;

        /**
         * S3 bucket
         */
        std::string s3Bucket;

        /**
         * Status
         */
        std::string status;

        /**
         * Version
         */
        std::string version;

      private:

        friend DescribeUserPoolDomainResponse tag_invoke(boost::json::value_to_tag<DescribeUserPoolDomainResponse>, boost::json::value const &v) {
            DescribeUserPoolDomainResponse r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.domain = Core::Json::GetStringValue(v, "Domain");
            r.s3Bucket = Core::Json::GetStringValue(v, "S3Bucket");
            r.status = Core::Json::GetStringValue(v, "Status");
            r.version = Core::Json::GetStringValue(v, "Version");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeUserPoolDomainResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"Domain", obj.domain},
                    {"S3Bucket", obj.s3Bucket},
                    {"Status", obj.status},
                    {"Version", obj.version},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_DOMAIN_RESPONSE_H
