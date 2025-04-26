//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/AuthenticationResult.h>
#include <awsmock/dto/cognito/model/ChallengeName.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Respond to an authentication challenge request
     *
     * Example:
     * @code{.json}
     * {
     *   "AuthenticationResult": {
     *      "AccessToken": "string",
     *      "ExpiresIn": number,
     *      "IdToken": "string",
     *      "NewDeviceMetadata": {
     *         "DeviceGroupKey": "string",
     *         "DeviceKey": "string"
     *      },
     *      "RefreshToken": "string",
     *      "TokenType": "string"
     *   },
     *   "ChallengeName": "string",
     *   "ChallengeParameters": {
     *      "string" : "string"
     *   },
     *   "Session": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RespondToAuthChallengeResponse final : Common::BaseDto<RespondToAuthChallengeResponse> {

        /**
         * The session that should be passed both ways in challenge-response calls to the service. If InitiateAuth or RespondToAuthChallenge API
         * call determines that the caller must pass another challenge, they return a session with other challenge parameters. This session
         * should be passed as it is to the next RespondToAuthChallenge API call.
         */
        std::string session;

        /**
         * Auth flow
         *
         * The challenge name. For more information, see InitiateAuth.
         */
        ChallengeName challengeName;

        /**
         * Challenge parameters
         */
        std::map<std::string, std::string> challengeParameters;

        /**
         * Auth parameters
         */
        AuthenticationResult authenticationResult;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_RESPONSE_H
