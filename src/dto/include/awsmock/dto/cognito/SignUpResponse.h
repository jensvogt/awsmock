//
// Created by vogje01 on 8/4/24.
//

#ifndef AWSMOCK_DTO_COGNITO_SIGN_UP_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_SIGN_UP_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Sign up response
     *
     * @code
     * {
     *   "CodeDeliveryDetails": {
     *      "AttributeName": "string",
     *      "DeliveryMedium": "string",
     *      "Destination": "string"
     *   },
     *   "UserConfirmed": boolean,
     *   "UserSub": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SignUpResponse : Common::BaseRequest {

        /**
         * User substitution
         *
         * The 128-bit ID of the authenticated user. This isn't the same as username.
         */
        std::string userSub;

        /**
         * User confirmed
         *
         * A response from the server indicating that a user registration has been confirmed.
         */
        bool userConfirmed;

        /**
         * @brief Convert from a JSON object.
         *
         * @param jsonString json string object
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const SignUpResponse &i);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_SIGN_UP_RESPONSE_H
