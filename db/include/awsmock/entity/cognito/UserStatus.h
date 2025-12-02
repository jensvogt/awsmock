//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_STATUS_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_STATUS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Database::Entity::Cognito {

    /**
     * @brief Cognito user status entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class UserStatus {
        UNCONFIRMED,
        CONFIRMED,
        EXTERNAL_PROVIDER,
        UNKNOWN,
        RESET_REQUIRED,
        FORCE_CHANGE_PASSWORD
    };

    static std::map<UserStatus, std::string> UserStatusNames{
            {UserStatus::UNCONFIRMED, "UNCONFIRMED"},
            {UserStatus::CONFIRMED, "CONFIRMED"},
            {UserStatus::EXTERNAL_PROVIDER, "EXTERNAL_PROVIDER"},
            {UserStatus::UNKNOWN, "UNKNOWN"},
            {UserStatus::RESET_REQUIRED, "RESET_REQUIRED"},
            {UserStatus::FORCE_CHANGE_PASSWORD, "FORCE_CHANGE_PASSWORD"},
    };

    [[maybe_unused]] static std::string UserStatusToString(const UserStatus &userStatus) {
        return UserStatusNames[userStatus];
    }

    [[maybe_unused]] static UserStatus UserStatusFromString(const std::string &userStatus) {
        for (auto &[fst, snd]: UserStatusNames) {
            if (snd == userStatus) {
                return fst;
            }
        }
        return UserStatus::UNKNOWN;
    }

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_STATUS_H
