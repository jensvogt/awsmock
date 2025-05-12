//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_ADD_USER_TO_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_ADD_USER_TO_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Adds a user to a group
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
    struct AdminAddUserToGroupRequest final : Common::BaseCounter<AdminAddUserToGroupRequest> {

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

        friend AdminAddUserToGroupRequest tag_invoke(boost::json::value_to_tag<AdminAddUserToGroupRequest>, boost::json::value const &v) {
            AdminAddUserToGroupRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.groupName = Core::Json::GetStringValue(v, "groupName");
            r.userName = Core::Json::GetStringValue(v, "userName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminAddUserToGroupRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userPoolId", obj.userPoolId},
                    {"groupName", obj.groupName},
                    {"userName", obj.userName},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_ADD_USER_TO_GROUP_REQUEST_H
