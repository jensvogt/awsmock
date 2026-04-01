//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_INITIATE_AUTH_REQUEST_H
#define AWSMOCK_DTO_COGNITO_ADMIN_INITIATE_AUTH_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Admin initiate auth request
     *
     * Example:
     * @code{.json}
     * {
     *    "AnalyticsMetadata": {
     *      "AnalyticsEndpointId": "string"
     *     },
     *     "AuthFlow": "string",
     *     "AuthParameters": {
     *     "string" : "string"
     *   },
     *   "ClientId": "string",
     *   "ClientMetadata": {
     *     "string" : "string"
     *   },
     *   "ContextData": {
     *   "EncodedData": "string",
     *   "HttpHeaders": [
     *     {
     *       "headerName": "string",
     *       "headerValue": "string"
     *     }
     *   ],
     *   "IpAddress": "string",
     *   "ServerName": "string",
     *   "ServerPath": "string"
     *   },
     *   "Session": "string",
     *   "UserPoolId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminInitiateAuthRequest final : Common::BaseCounter<AdminInitiateAuthRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * @brief Session
         *
         * The optional session ID from a ConfirmSignUp API request. You can sign in a user directly from the sign-up process with an AuthFlow
         * of USER_AUTH and AuthParameters of EMAIL_OTP or SMS_OTP, depending on how your user pool sent the confirmation-code message.
         */
        std::string session;

        /**
         * Authentication flow
         */
        std::string authFlow;

        /**
         * AuthParameters
         *
         * The authentication parameters. These are inputs corresponding to the AuthFlow that you're invoking. the following are some authentication flows and their
         * parameters. Add a SECRET_HASH parameter if your app client has a client secret. Add DEVICE_KEY if you want to bypass multi-factor authentication with a remembered device.
         * USER_AUTH
         *   USERNAME (required)
         * PREFERRED_CHALLENGE. If you don't provide a value for PREFERRED_CHALLENGE, Amazon Cognito responds with the AvailableChallenges parameter that specifies the available sign-in methods.
         *
         * USER_SRP_AUTH
         *   USERNAME (required)
         *   SRP_A (required)
         *
         * ADMIN_USER_PASSWORD_AUTH
         *   USERNAME (required)
         *   PASSWORD (required)
         *
         * REFRESH_TOKEN_AUTH/REFRESH_TOKEN
         *   REFRESH_TOKEN(required)
         *
         * CUSTOM_AUTH
         *   USERNAME (required)
         *   ChallengeName: SRP_A (when preceding custom authentication with SRP authentication)
         *   SRP_A: (An SRP_A value) (when preceding custom authentication with SRP authentication)
         *
         * For more information about SECRET_HASH, see Computing secret hash values. For information about DEVICE_KEY, see Working with user devices in your user pool.
         */
        std::map<std::string, std::string> authParameters;

        /**
         * @brief Client ID
         *
         * @par
         * The ID of the app client where the user wants to sign in.
         */
        std::string clientId;

        /**
         * Client metadata
         */
        std::map<std::string, std::string> clientMetadata;

      private:

        friend AdminInitiateAuthRequest tag_invoke(boost::json::value_to_tag<AdminInitiateAuthRequest>, boost::json::value const &v) {
            AdminInitiateAuthRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.session = Core::Json::GetStringValue(v, "Session");
            r.authFlow = Core::Json::GetStringValue(v, "AuthFlow");
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            if (Core::Json::AttributeExists(v, "AuthParameters")) {
                r.authParameters = boost::json::value_to<std::map<std::string, std::string>>(v.at("AuthParameters"));
            }
            if (Core::Json::AttributeExists(v, "ClientMetadata")) {
                r.clientMetadata = boost::json::value_to<std::map<std::string, std::string>>(v.at("ClientMetadata"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminInitiateAuthRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Session", obj.userPoolId},
                    {"Session", obj.session},
                    {"AuthFlow", obj.authFlow},
                    {"ClientId", obj.clientId},
                    {"AuthParameters", boost::json::value_from(obj.authParameters)},
                    {"ClientMetadata", boost::json::value_from(obj.clientMetadata)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_INITIATE_AUTH_REQUEST_H
