//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List users request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersRequest final : Common::BaseCounter<ListUsersRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

      private:

        friend ListUsersRequest tag_invoke(boost::json::value_to_tag<ListUsersRequest>, boost::json::value const &v) {
            ListUsersRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUsersRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERS_REQUEST_H
