//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_CLIENT_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_CLIENT_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/cognito/model/UserPoolClient.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create user pool client response
     *
     * Example:
     * @code{.json}
     * {
     *   "UserPoolClient": {
     *      "AccessTokenValidity": number,
     *      "AllowedOAuthFlows": [ "string" ],
     *      "AllowedOAuthFlowsUserPoolClient": boolean,
     *      "AllowedOAuthScopes": [ "string" ],
     *      "AnalyticsConfiguration": {
     *         "ApplicationArn": "string",
     *         "ApplicationId": "string",
     *         "ExternalId": "string",
     *         "RoleArn": "string",
     *         "UserDataShared": boolean
     *      },
     *      "AuthSessionValidity": number,
     *      "CallbackURLs": [ "string" ],
     *      "ClientId": "string",
     *      "ClientName": "string",
     *      "ClientSecret": "string",
     *      "CreationDate": number,
     *      "DefaultRedirectURI": "string",
     *      "EnablePropagateAdditionalUserContextData": boolean,
     *      "EnableTokenRevocation": boolean,
     *      "ExplicitAuthFlows": [ "string" ],
     *      "IdTokenValidity": number,
     *      "LastModifiedDate": number,
     *      "LogoutURLs": [ "string" ],
     v      "PreventUserExistenceErrors": "string",
     *      "ReadAttributes": [ "string" ],
     *      "RefreshTokenValidity": number,
     *      "SupportedIdentityProviders": [ "string" ],
     *      "TokenValidityUnits": {
     *         "AccessToken": "string",
     *         "IdToken": "string",
     *         "RefreshToken": "string"
     *      },
     *      "UserPoolId": "string",
     *      "WriteAttributes": [ "string" ]
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateUserPoolClientResponse final : Common::BaseCounter<CreateUserPoolClientResponse> {

        /**
         * @brief User group client
         */
        UserPoolClient userGroupClient;

      private:

        friend CreateUserPoolClientResponse tag_invoke(boost::json::value_to_tag<CreateUserPoolClientResponse>, boost::json::value const &v) {
            CreateUserPoolClientResponse r;
            r.userGroupClient = boost::json::value_to<UserPoolClient>(v.at("UserPoolClient"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateUserPoolClientResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserGroupClient", boost::json::value_from(obj.userGroupClient)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CREATE_USERPOOL_CLIENT_RESPONSE_H
