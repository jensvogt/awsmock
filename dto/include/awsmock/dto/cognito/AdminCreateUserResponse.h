//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/User.h>
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create a user response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminCreateUserResponse final : Common::BaseCounter<AdminCreateUserResponse> {

        /**
         * Name of the user
         */
        User user;

      private:

        friend AdminCreateUserResponse tag_invoke(boost::json::value_to_tag<AdminCreateUserResponse>, boost::json::value const &v) {
            AdminCreateUserResponse r;
            r.user = boost::json::value_to<User>(v.at("User"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminCreateUserResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"RequestId", obj.requestId},
                    {"User", boost::json::value_from(obj.user)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
