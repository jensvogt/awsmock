//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_CLIENT_REQUEST_H
#define AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_CLIENT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/cognito/model/TokenValidityUnits.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create a user pool client request.
     *
     * Request to create a new user pool client.
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
     *   "ClientName": "string",
     *   "DefaultRedirectURI": "string",
     *   "EnablePropagateAdditionalUserContextData": boolean,
     *   "EnableTokenRevocation": boolean,
     *   "ExplicitAuthFlows": [ "string" ],
     *   "GenerateSecret": boolean,
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
    struct CreateUserPoolClientRequest final : Common::BaseCounter<CreateUserPoolClientRequest> {

        /**
         * User pool of the domain
         */
        std::string userPoolId;

        /**
         * Client name
         */
        std::string clientName;

        /**
         * Generate secret flag
         */
        bool generateSecret = true;

        /**
         * Access token validity
         *
         * Default 8 hours
         */
        long accessTokenValidity = 8;

        /**
         * ID token validity
         *
         * Default 8 hours
         */
        long idTokenValidity = 8;

        /**
         * Refresh token validity
         *
         * Default 8 hours
         */
        long refreshTokenValidity = 8;

        /**
         * Auth session validity
         *
         * Default 8 hours
         */
        long authSessionValidity = 8;

        /**
         * Token validity units
         */
        TokenValidityUnits tokenValidityUnits;

      private:

        friend CreateUserPoolClientRequest tag_invoke(boost::json::value_to_tag<CreateUserPoolClientRequest>, boost::json::value const &v) {
            CreateUserPoolClientRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "UserPoolId");
            r.clientName = Core::Json::GetStringValue(v, "ClientName");
            r.generateSecret = Core::Json::GetBoolValue(v, "GenerateSecret");
            r.accessTokenValidity = Core::Json::GetLongValue(v, "AccessTokenValidity");
            r.idTokenValidity = Core::Json::GetLongValue(v, "IdTokenValidity");
            r.refreshTokenValidity = Core::Json::GetLongValue(v, "RefreshTokenValidity");
            r.authSessionValidity = Core::Json::GetLongValue(v, "AuthSessionValidity");
            r.tokenValidityUnits = boost::json::value_to<TokenValidityUnits>(v, "TokenValidityUnits");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUserPoolClientRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolId", obj.userPoolId},
                    {"ClientName", obj.clientName},
                    {"GenerateSecret", obj.generateSecret},
                    {"AccessTokenValidity", obj.accessTokenValidity},
                    {"IdTokenValidity", obj.idTokenValidity},
                    {"AuthSessionValidity", obj.authSessionValidity},
                    {"TokenValidityUnits", boost::json::value_from(obj.idTokenValidity)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_CLIENT_REQUEST_H
