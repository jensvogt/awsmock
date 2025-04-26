//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Get admin user response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminGetUserResponse {

        /**
         * ID user
         */
        std::string id;

        /**
         * AWS region
         */
        std::string region;

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
        Database::Entity::Cognito::UserStatus userStatus;

        /**
         * User userAttributes list
         */
        UserAttributeList userAttributes;

        /**
         * Created
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Modified
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Convert from a JSON object.
         *
         * @param payload json string object
         */
        void FromJson(const std::string &payload);

        /**
         * @brief Convert from a JSON object.
         *
         * @return JSON representation of the object
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const AdminGetUserResponse &i);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_GET_USER_RESPONSE_H
