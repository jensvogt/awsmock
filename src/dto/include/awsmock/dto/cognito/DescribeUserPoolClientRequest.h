//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_REQUEST_H

// C++ standard includes

#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

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
    struct DescribeUserPoolClientRequest final : Common::BaseCounter<DescribeUserPoolClientRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User pool ID
         */
        std::string clientId;

      private:

        friend DescribeUserPoolClientRequest tag_invoke(boost::json::value_to_tag<DescribeUserPoolClientRequest>, boost::json::value const &v) {
            DescribeUserPoolClientRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeUserPoolClientRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"ClientId", obj.clientId},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_REQUEST_H
