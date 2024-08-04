//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_AUTHENTICATION_RESULT_H
#define AWSMOCK_DTO_COGNITO_MODEL_AUTHENTICATION_RESULT_H

// C++ includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    struct AuthenticationResult {

        /**
         * Access token
         */
        std::string accessToken;

        /**
         * ID token
         */
        std::string idToken;

        /**
         * Refresh token
         */
        std::string refreshToken;

        /**
         * Token type
         */
        std::string tokenType;

        /**
         * Expires in
         */
        long expiredIn;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const AuthenticationResult &r);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_AUTHENTICATION_RESULT_H
