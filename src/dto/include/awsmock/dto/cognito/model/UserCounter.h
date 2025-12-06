//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_USER_COUNTER_H
#define AWSMOCK_DTO_COGNITO_USER_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/cognito/model/Group.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/entity/cognito/UserAttribute.h>
#include <awsmock/entity/cognito/UserStatus.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserCounter final : Common::BaseCounter<UserCounter> {

        /**
         * MongoDB OID
         */
        std::string oid;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User name
         */
        std::string userName;

        /**
         * Enabled
         */
        bool enabled = false;

        /**
         * Attributes
         */
        //UserAttributeList userAttributes;

        /**
         * Status
         */
        Database::Entity::Cognito::UserStatus userStatus;

        /**
         * Password
         */
        std::string password;

        /**
         * Groups
         */
        std::vector<Group> groups;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Converts the entity to a JSON object
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the entity to a JSON object
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document);

      private:

        friend UserCounter tag_invoke(boost::json::value_to_tag<UserCounter>, boost::json::value const &v) {
            UserCounter r;
            r.oid = v.at("oid").as_string();
            r.userPoolId = v.at("userPoolId").as_string();
            r.userName = v.at("userName").as_string();
            r.password = v.at("password").as_string();
            r.enabled = v.at("enabled").as_bool();
            r.userStatus = Database::Entity::Cognito::UserStatusFromString(v.at("userName").as_string().data());
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"oid", obj.oid},
                    {"userPoolId", obj.userPoolId},
                    {"userName", obj.userName},
                    {"password", obj.password},
                    {"enabled", obj.enabled},
                    {"userStatus", Database::Entity::Cognito::UserStatusToString(obj.userStatus)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_USER_COUNTER_H
