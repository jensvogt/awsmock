//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create user response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminCreateUserResponse : Common::BaseRequest {

        /**
         * Name of the user
         */
        std::string userName;

        /**
         * Enabled flag
         */
        bool enabled = false;

        /**
         * User userAttributes list
         */
        UserAttributeList userAttributes;

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
        friend std::ostream &operator<<(std::ostream &os, const AdminCreateUserResponse &i);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_CREATE_USER_RESPONSE_H
