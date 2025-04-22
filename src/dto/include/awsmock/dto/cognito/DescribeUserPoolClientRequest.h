//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_REQUEST_H

// C++ standard includes

#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Describe user pool client request.
     *
     * Request to describe a user pool client.
     *
     * Example:
     * @code{.json}
     * {
     *   "UserPoolId": "string"
     *   "ClientId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeUserPoolClientRequest final : Common::BaseDto<DescribeUserPoolClientRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User pool ID
         */
        std::string clientId;

        /**
         * @brief Convert from a JSON object.
         *
         * @param jsonString json string object
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_REQUEST_H
