//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/cognito/model/UserPoolClient.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Create user pool response
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
     *      "PreventUserExistenceErrors": "string",
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
    struct DescribeUserPoolClientResponse final : Common::BaseCounter<DescribeUserPoolClientResponse> {

        /**
         * User pool client
         */
        UserPoolClient userPoolClient;

      private:

        friend DescribeUserPoolClientResponse tag_invoke(boost::json::value_to_tag<DescribeUserPoolClientResponse>, boost::json::value const &v) {
            DescribeUserPoolClientResponse r;
            r.userPoolClient = boost::json::value_to<UserPoolClient>(v.at("UserPoolClient"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeUserPoolClientResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"UserPoolClient", boost::json::value_from(obj.userPoolClient)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_DESCRIBE_USERPOOL_CLIENT_RESPONSE_H
