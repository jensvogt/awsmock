//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_USER_H
#define AWSMOCK_DTO_COGNITO_USER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/cognito/model/Group.h>
#include <awsmock/entity/cognito/UserStatus.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct User final : Common::BaseCounter<User> {

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
        Database::Entity::Cognito::UserStatus userStatus = Database::Entity::Cognito::UserStatus::UNKNOWN;

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "region", region);
                Core::Bson::BsonUtils::SetStringValue(document, "userName", userName);
                Core::Bson::BsonUtils::SetStringValue(document, "userPoolId", userPoolId);
                Core::Bson::BsonUtils::SetStringValue(document, "userStatus", Database::Entity::Cognito::UserStatusToString(userStatus));
                Core::Bson::BsonUtils::SetBoolValue(document, "enabled", enabled);
                Core::Bson::BsonUtils::SetDateValue(document, "created", created);
                Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the entity to a JSON object
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                region = Core::Bson::BsonUtils::GetStringValue(document, "region");
                userName = Core::Bson::BsonUtils::GetStringValue(document, "userName");
                userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "userPoolId");
                enabled = Core::Bson::BsonUtils::GetBoolValue(document, "enabled");
                userStatus = Database::Entity::Cognito::UserStatusFromString(Core::Bson::BsonUtils::GetStringValue(document, "userStatus"));

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend User tag_invoke(boost::json::value_to_tag<User>, boost::json::value const &v) {
            User r;
            r.oid = Core::Json::GetStringValue(v, "Oid");
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.userName = Core::Json::GetStringValue(v, "Username");
            r.enabled = Core::Json::GetBoolValue(v, "Enabled");
            r.userStatus = Database::Entity::Cognito::UserStatusFromString(Core::Json::GetStringValue(v, "UserStatus"));
            r.password = Core::Json::GetStringValue(v, "Password");
            if (Core::Json::AttributeExists(v, "Groups")) {
                r.groups = boost::json::value_to<std::vector<Group>>(v.at("Groups"));
            }
            r.created = Core::DateTimeUtils::FromISO8601(v.at("Created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("Modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, User const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Oid", obj.oid},
                    {"UserPoolId", obj.userPoolId},
                    {"Username", obj.userName},
                    {"Enabled", obj.enabled},
                    {"UserStatus", Database::Entity::Cognito::UserStatusToString(obj.userStatus)},
                    {"Password", boost::json::value_from(obj.password)},
                    {"Groups", boost::json::value_from("groups")},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

    typedef std::vector<User> UserList;

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_USER_H
