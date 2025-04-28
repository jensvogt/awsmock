//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_GLOBAL_SIGN_OUT_REQUEST_H
#define AWSMOCK_DTO_COGNITO_GLOBAL_SIGN_OUT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Global sign out request
     *
     * @par
     * Invalidates the identity, access, and refresh tokens that Amazon Cognito issued to a user. Call this operation when your user signs out of your app. This
     * results in the following behavior. Amazon Cognito no longer accepts token-authorized user operations that you authorize with a signed-out user's access
     * tokens. For more information, see Using the Amazon Cognito user pools API and user pool endpoints. Amazon Cognito returns an Access Token has been revoked
     * error when your app attempts to authorize a user pools API request with a revoked access token that contains the scope aws.cognito.signin.user.admin.
     * Amazon Cognito no longer accepts a signed-out user's ID token in a GetId request to an identity pool with ServerSideTokenCheck enabled for its user pool
     * IdP configuration in CognitoIdentityProvider. Amazon Cognito no longer accepts a signed-out user's refresh tokens in refresh requests. Other requests
     * might be valid until your user's token expires. Authorize this action with a signed-in user's access token. It must include the scope
     * aws.cognito.signin.user.admin.
     *
     * Example:
     * @code{.json}
     * {
     *   "AccessToken": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GlobalSignOutRequest final : Common::BaseDto<GlobalSignOutRequest> {

        /**
         * A valid access token that Amazon Cognito issued to the user who you want to sign out.
         */
        std::string accessToken;

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
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_GLOBAL_SIGN_OUT_REQUEST_H
