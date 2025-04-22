//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_INITIATE_AUTH_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_INITIATE_AUTH_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/AuthenticationResult.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Initiate authentication request
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
    struct InitiateAuthResponse final : Common::BaseDto<InitiateAuthResponse> {

        /**
         * Authentication result
         */
        AuthenticationResult authenticationResult;

        /**
         * Available challenges
         */
        std::vector<std::string> availableChallenges;

        /**
         * Session
         */
        std::string session;

        /**
         * Client ID
         */
        std::string clientId;

        /**
         * Challenge name
         */
        std::string challengeName;

        /**
         * Challenge parameters
         */
        std::map<std::string, std::string> challengeParameters;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_INITIATE_AUTH_RESPONSE_H
