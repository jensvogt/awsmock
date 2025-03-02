//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DB_ENTITY_COGNITO_USER_H
#define AWSMOCK_DB_ENTITY_COGNITO_USER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/cognito/Group.h>
#include <awsmock/entity/cognito/UserAttribute.h>
#include <awsmock/entity/cognito/UserStatus.h>

namespace AwsMock::Database::Entity::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct User {

        /**
         * MongoDB OID
         */
        std::string oid;

        /**
         * Aws region
         */
        std::string region;

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
        bool enabled;

        /**
         * Attributes
         */
        UserAttributeList userAttributes;

        /**
         * Status
         */
        UserStatus userStatus;

        /**
         * Password
         */
        std::string password;

        /**
         * Confirmation code
         */
        std::string confirmationCode;

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
         * @brief Checks whether the user has already a group
         *
         * @param userPoolId user pool ID
         * @param groupName name of the group
         */
        bool HasGroup(const std::string &userPoolId, const std::string &groupName);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult query result.
         */
        void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the entity to a string representation.
         *
         * @return entity as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param user user entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const User &user);
    };

    typedef std::vector<User> UserList;

}// namespace AwsMock::Database::Entity::Cognito

#endif// AWSMOCK_DB_ENTITY_COGNITO_USER_H
