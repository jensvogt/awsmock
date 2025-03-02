//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_CLIENT_H
#define AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_CLIENT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/TokenValidityUnits.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user pool client
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPoolClient {

        /**
         * ID
         */
        std::string id;

        /**
         * Region
         */
        std::string region;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Client ID
         */
        std::string clientId;

        /**
         * Client name
         */
        std::string clientName;

        /**
         * Client secret
         */
        std::string clientSecret;

        /**
         * Access token validity
         */
        long accessTokenValidity;

        /**
         * ID token validity
         */
        long idTokenValidity;

        /**
         * Refresh token validity
         */
        long refreshTokenValidity;

        /**
         * Token validity units
         */
        TokenValidityUnits tokenValidityUnits;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Last modified
         */
        system_clock::time_point modified;

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
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

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
        friend std::ostream &operator<<(std::ostream &os, const UserPoolClient &r);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_CLIENT_H
