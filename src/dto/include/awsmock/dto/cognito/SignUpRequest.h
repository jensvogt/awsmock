//
// Created by vogje01 on 8/4/24.
//

#ifndef AWSMOCK_DTO_COGNITO_SIGN_UP_REQUEST_H
#define AWSMOCK_DTO_COGNITO_SIGN_UP_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/UserAttribute.h>
#include <awsmock/dto/cognito/model/UserContextData.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Sign up request
     *
     * @code
     * {
     *   "AnalyticsMetadata": {
     *      "AnalyticsEndpointId": "string"
     *   },
     *   "ClientId": "string",
     *   "ClientMetadata": {
     *      "string" : "string"
     *   },
     *   "Password": "string",
     *   "SecretHash": "string",
     *   "UserAttributes": [
     *      {
     *         "Name": "string",
     *         "Value": "string"
     *      }
     *   ],
     *   "UserContextData": {
     *      "EncodedData": "string",
     *      "IpAddress": "string"
     *   },
     *   "Username": "string",
     *   "ValidationData": [
     *      {
     *         "Name": "string",
     *         "Value": "string"
     *      }
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SignUpRequest : Common::BaseRequest<SignUpRequest> {

        /**
         * Client ID
         *
         * The ID of the client associated with the user pool.
         */
        std::string clientId;

        /**
         * Username
         *
         * The username of the user that you want to sign up. The value of this parameter is typically a username, but can be any alias attribute in your user pool.
         */
        std::string userName;

        /**
         * Password
         *
         * The password of the user you want to register.
         */
        std::string password;

        /**
         * Secret hash
         *
         * A keyed-hash message authentication code (HMAC) calculated using the secret key of a user pool client and username plus the client ID in the message.
         */
        std::string secretHash;

        /**
         * User attributes
         *
         * An array of name-value pairs representing user attributes.
         */
        std::vector<UserAttribute> userAttributes;

        /**
         * User context data
         *
         * Contextual data about your user session, such as the device fingerprint, IP address, or location. Amazon Cognito advanced security evaluates the risk of
         * an authentication event based on the context that your app generates and passes to Amazon Cognito when it makes API requests.
         */
        UserContextData userContextData;

        /**
         * Client metadata
         *
         * A map of custom key-value pairs that you can provide as input for any custom workflows that this action triggers.
         */
        std::map<std::string, std::string> clientMetadata;

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
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_SIGN_UP_REQUEST_H
