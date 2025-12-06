//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
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
        std::string userName;

        /**
         * Enabled flag
         */
        bool enabled = false;

        /**
         * User attributes list
         */
        std::vector<UserAttribute> userAttributes;

      private:

        friend AdminCreateUserResponse tag_invoke(boost::json::value_to_tag<AdminCreateUserResponse>, boost::json::value const &v) {
            AdminCreateUserResponse r;
            r.userName = Core::Json::GetStringValue(v, "userName");
            if (Core::Json::AttributeExists(v, "userAttributes")) {
                r.userAttributes = boost::json::value_to<std::vector<UserAttribute>>(v, "userAttributes");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminCreateUserResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userName", obj.userName},
                    {"enabled", obj.enabled},
                    {"userAttributes", boost::json::value_from(obj.userAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
