//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_REQUEST_H
#define AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Describe user pool request
     *
     * Request to describe a user pool.
     *
     * Example:
     * @code{.json}
     * {
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeUserPoolRequest final : Common::BaseCounter<DescribeUserPoolRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

      private:

        friend DescribeUserPoolRequest tag_invoke(boost::json::value_to_tag<DescribeUserPoolRequest>, boost::json::value const &v) {
            DescribeUserPoolRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeUserPoolRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_REQUEST_H
