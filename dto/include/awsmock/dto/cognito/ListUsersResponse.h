//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERS_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/UserCounter.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersResponse final : Common::BaseCounter<ListUsersResponse> {

        /**
         * User entities
         */
        std::vector<User> users;

        /**
         * User entities
         */
        long total{};

      private:

        friend ListUsersResponse tag_invoke(boost::json::value_to_tag<ListUsersResponse>, boost::json::value const &v) {
            ListUsersResponse r;
            r.users = boost::json::value_to<std::vector<User>>(v.at("Users"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUsersResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Users", boost::json::value_from(obj.users)},
                    {"Total", obj.total},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERS_RESPONSE_H
