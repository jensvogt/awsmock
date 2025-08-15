//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Admin get user request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminGetUserRequest final : Common::BaseCounter<AdminGetUserRequest> {

        /**
         * ID of the user pool
         */
        std::string userPoolId;

        /**
         * Name of the user
         */
        std::string userName;

      private:

        friend AdminGetUserRequest tag_invoke(boost::json::value_to_tag<AdminGetUserRequest>, boost::json::value const &v) {
            AdminGetUserRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.userName = Core::Json::GetStringValue(v, "Username");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminGetUserRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"Username", obj.userName},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_REQUEST_H
