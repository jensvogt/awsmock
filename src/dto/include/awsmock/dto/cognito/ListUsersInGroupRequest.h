//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USER_IN_GROUP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USER_IN_GROUP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users in a group request
     *
     * Example:
     * @code{.json}
     * {
     *   "GroupName": "string",
     *   "Limit": number,
     *   "NextToken": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersInGroupRequest final : Common::BaseCounter<ListUsersInGroupRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Name of the group
         */
        std::string groupName;

        /**
         * Limit
         */
        long limit;

        /**
         * Next token
         */
        std::string nextToken;

      private:

        friend ListUsersInGroupRequest tag_invoke(boost::json::value_to_tag<ListUsersInGroupRequest>, boost::json::value const &v) {
            ListUsersInGroupRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.groupName = Core::Json::GetStringValue(v, "GroupName");
            r.limit = Core::Json::GetLongValue(v, "Limit");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUsersInGroupRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"GroupName", obj.groupName},
                    {"NextToken", obj.nextToken},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USER_IN_GROUP_REQUEST_H
