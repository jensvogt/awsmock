//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERS_IN_GROUP_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERS_IN_GROUP_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/cognito/model/User.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users in a group response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersInGroupResponse final : Common::BaseCounter<ListUsersInGroupResponse> {

        /**
         * User pool entities
         */
        std::vector<User> users;

      private:

        friend ListUsersInGroupResponse tag_invoke(boost::json::value_to_tag<ListUsersInGroupResponse>, boost::json::value const &v) {
            ListUsersInGroupResponse r;
            r.users = boost::json::value_to<std::vector<User>>(v.at("Users"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUsersInGroupResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Users", boost::json::value_from(obj.users)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERS_IN_GROUP_RESPONSE_H
