//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/MessageAction.h>
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create user request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminCreateUserRequest final : Common::BaseCounter<AdminCreateUserRequest> {

        /**
         * ID of the user pool
         */
        std::string userPoolId;

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * Message action
         */
        MessageAction messageAction = SUPPRESS;

        /**
         * Temporary password
         */
        std::string temporaryPassword;

        /**
         * User userAttributes list
         */
        std::vector<UserAttribute> userAttributes;

      private:

        friend AdminCreateUserRequest tag_invoke(boost::json::value_to_tag<AdminCreateUserRequest>, boost::json::value const &v) {
            AdminCreateUserRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.userName = Core::Json::GetStringValue(v, "userName");
            r.messageAction = MessageActionFromString(Core::Json::GetStringValue(v, "messageAction"));
            r.temporaryPassword = Core::Json::GetStringValue(v, "temporaryPassword");
            if (Core::Json::AttributeExists(v, "userAttributes")) {
                r.userAttributes = boost::json::value_to<std::vector<UserAttribute>>(v, "userAttributes");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminCreateUserRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userPoolId", obj.userPoolId},
                    {"userName", obj.userName},
                    {"messageAction", MessageActionToString(obj.messageAction)},
                    {"temporaryPassword", obj.temporaryPassword},
                    {"userAttributes", boost::json::value_from(obj.userAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_REQUEST_H
