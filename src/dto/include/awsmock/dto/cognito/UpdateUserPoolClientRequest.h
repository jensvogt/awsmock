//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_CLIENT_REQUEST_H
#define AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_CLIENT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/TokenValidityUnits.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Updates an existing user pool request
     *
     * Request to update an existing user pool.
     *
     * Example:
     * @code{.json}
     * {
     *   "AccessTokenValidity": number,
     *   "AllowedOAuthFlows": [ "string" ],
     *   "AllowedOAuthFlowsUserPoolClient": boolean,
     *   "AllowedOAuthScopes": [ "string" ],
     *   "AnalyticsConfiguration": {
     *      "ApplicationArn": "string",
     *      "ApplicationId": "string",
     *      "ExternalId": "string",
     *      "RoleArn": "string",
     *      "UserDataShared": boolean
     *   },
     *   "AuthSessionValidity": number,
     *   "CallbackURLs": [ "string" ],
     *   "ClientId": "string",
     *   "ClientName": "string",
     *   "DefaultRedirectURI": "string",
     *   "EnablePropagateAdditionalUserContextData": boolean,
     *   "EnableTokenRevocation": boolean,
     *   "ExplicitAuthFlows": [ "string" ],
     *   "IdTokenValidity": number,
     *   "LogoutURLs": [ "string" ],
     *   "PreventUserExistenceErrors": "string",
     *   "ReadAttributes": [ "string" ],
     *   "RefreshTokenValidity": number,
     *   "SupportedIdentityProviders": [ "string" ],
     *   "TokenValidityUnits": {
     *      "AccessToken": "string",
     *      "IdToken": "string",
     *      "RefreshToken": "string"
     *   },
     *   "UserPoolId": "string",
     *   "WriteAttributes": [ "string" ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateUserPoolClientRequest final : Common::BaseCounter<UpdateUserPoolClientRequest> {

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
         * Access token validity
         */
        int accessTokenValidity;

        /**
         * ID token validity
         */
        int idTokenValidity;

        /**
         * Refresh token validity
         */
        int refreshTokenValidity;

        /**
         * Auth session validity
         */
        int authSessionValidity;

        /**
         * Token validity units
         */
        TokenValidityUnits tokenValidityUnits;

      private:

        friend UpdateUserPoolClientRequest tag_invoke(boost::json::value_to_tag<UpdateUserPoolClientRequest>, boost::json::value const &v) {
            UpdateUserPoolClientRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            r.clientName = Core::Json::GetStringValue(v, "ClientName");
            r.accessTokenValidity = Core::Json::GetLongValue(v, "AccessTokenValidity");
            r.idTokenValidity = Core::Json::GetLongValue(v, "IdTokenValidity");
            r.refreshTokenValidity = Core::Json::GetLongValue(v, "RefreshTokenValidity");
            r.authSessionValidity = Core::Json::GetLongValue(v, "AuthSessionValidity");
            r.tokenValidityUnits = boost::json::value_to<TokenValidityUnits>(v.at("TokenValidityUnits"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateUserPoolClientRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"ClientId", obj.clientId},
                    {"ClientName", obj.clientName},
                    {"AccessTokenValidity", obj.accessTokenValidity},
                    {"IdTokenValidity", obj.idTokenValidity},
                    {"RefreshTokenValidity", obj.refreshTokenValidity},
                    {"AuthSessionValidity", obj.authSessionValidity},
                    {"TokenValidityUnits", boost::json::value_from(obj.tokenValidityUnits)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_UPDATE_USERPOOL_CLIENT_REQUEST_H
