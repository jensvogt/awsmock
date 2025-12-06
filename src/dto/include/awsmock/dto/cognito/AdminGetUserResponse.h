//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Get admin user response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminGetUserResponse final : Common::BaseCounter<AdminGetUserResponse> {

        /**
         * ID user
         */
        std::string id;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * Password of the user
         */
        std::string password;

        /**
         * Enabled flag
         */
        bool enabled = false;

        /**
         * User status
         */
        Database::Entity::Cognito::UserStatus userStatus = Database::Entity::Cognito::UserStatus::UNKNOWN;

        /**
         * User attributes list
         */
        std::vector<UserAttribute> userAttributes;

        /**
         * Created
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Modified
         */
        system_clock::time_point modified = system_clock::now();

      private:

        friend AdminGetUserResponse tag_invoke(boost::json::value_to_tag<AdminGetUserResponse>, boost::json::value const &v) {
            AdminGetUserResponse r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.userName = Core::Json::GetStringValue(v, "Username");
            r.password = Core::Json::GetStringValue(v, "Password");
            r.enabled = Core::Json::GetBoolValue(v, "Enabled");
            r.userStatus = Database::Entity::Cognito::UserStatusFromString(Core::Json::GetStringValue(v, "UserStatus"));
            if (Core::Json::AttributeExists(v, "UserAttributes")) {
                r.userAttributes = boost::json::value_to<std::vector<UserAttribute>>(v, "UserAttributes");
            }
            r.created = Core::DateTimeUtils::FromISO8601(v.at("Created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("Modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminGetUserResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Id", obj.id},
                    {"UserPoolId", obj.userPoolId},
                    {"Username", obj.userName},
                    {"Password", obj.password},
                    {"Enabled", obj.enabled},
                    {"UserStatus", Database::Entity::Cognito::UserStatusToString(obj.userStatus)},
                    {"UserAttributes", boost::json::value_from(obj.userAttributes)},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},

            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_RESPONSE_H
