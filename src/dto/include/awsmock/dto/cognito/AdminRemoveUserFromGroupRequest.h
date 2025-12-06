//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_REMOVE_USER_FROM_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_REMOVE_USER_FROM_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Removes a user from a group
     *
     * Example:
     * @code{.json}
     * {
     *   "GroupName": "string",
     *   "Username": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminRemoveUserFromGroupRequest final : Common::BaseCounter<AdminRemoveUserFromGroupRequest> {

        /**
         * Name of the group
         */
        std::string groupName;

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * User pool id
         */
        std::string userPoolId;

      private:

        friend AdminRemoveUserFromGroupRequest tag_invoke(boost::json::value_to_tag<AdminRemoveUserFromGroupRequest>, boost::json::value const &v) {
            AdminRemoveUserFromGroupRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.userName = Core::Json::GetStringValue(v, "userName");
            r.userName = Core::Json::GetStringValue(v, "userName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminRemoveUserFromGroupRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userPoolId", obj.userPoolId},
                    {"userName", obj.userName},
                    {"groupName", obj.groupName},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_REMOVE_USER_FROM_GROUP_REQUEST_H
