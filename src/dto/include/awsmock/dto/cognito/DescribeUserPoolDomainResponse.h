//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_DOMAIN_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_DOMAIN_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/cognito/model/UserPoolClient.h>
#include <awsmock/dto/common/BaseRequest.h>

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
    struct DescribeUserPoolDomainResponse : public Dto::Common::BaseRequest {

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

        /**
         * Convert to a JSON string.
         *
         * @return json string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const DescribeUserPoolDomainResponse &i);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_DOMAIN_RESPONSE_H