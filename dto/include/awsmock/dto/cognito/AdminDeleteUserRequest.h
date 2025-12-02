//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_DELETE_USER_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_DELETE_USER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Delete user request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminDeleteUserRequest final : Common::BaseCounter<AdminDeleteUserRequest> {

        /**
         * ID of the user pool
         */
        std::string userPoolId;

        /**
         * Name of the user
         */
        std::string userName;

    private:

        friend AdminDeleteUserRequest tag_invoke(boost::json::value_to_tag<AdminDeleteUserRequest>, boost::json::value const &v) {
            AdminDeleteUserRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.userName = Core::Json::GetStringValue(v, "Username");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminDeleteUserRequest const &obj) {
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

#endif// AWSMOCK_DTO_COGNITO_ADMIN_DELETE_USER_REQUEST_H
