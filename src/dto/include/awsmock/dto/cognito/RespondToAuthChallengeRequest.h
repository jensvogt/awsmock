//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_REQUEST_H
#define AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/cognito/model/ChallengeName.h>
#include <awsmock/dto/cognito/model/UserContextData.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Responde to an authentication challenge
     *
     * Example:
     * @code{.json}
     * {
     *   "AnalyticsMetadata": {
     *      "AnalyticsEndpointId": "string"
     *   },
     *   "ChallengeName": "string",
     *   "ChallengeResponses": {
     *      "string" : "string"
     *   },
     *   "ClientId": "string",
     *   "ClientMetadata": {
     *      "string" : "string"
     *   },
     *   "Session": "string",
     *   "UserContextData": {
     *      "EncodedData": "string",
     *      "IpAddress": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RespondToAuthChallengeRequest final : Common::BaseDto<RespondToAuthChallengeRequest> {

        /**
         * The app client ID.
         */
        std::string clientId;

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
         * The responses to the challenge that you received in the previous request. Each challenge has its own required response parameters. The
         * following examples are partial JSON request bodies that highlight challenge-response parameters.
         */
        std::map<std::string, std::string> challengeResponses;

        /**
         * Client metadata
         */
        std::map<std::string, std::string> clientMetaData;

        /**
         * User context data
         */
        UserContextData userContextData;

        /**
         * @brief Returns the username
         *
         * @return user ID
         */
        std::string GetUserName();

        /**
         * @brief Returns the user password
         *
         * @return user password
         */
        std::string GetPasswordClaim_Signature();

        /**
         * @brief Returns the client secret from the AuthParameters
         *
         * @return client secret
         */
        std::string GetPasswordClaimSecretBlock();

        /**
         * @brief Returns the username
         *
         * @return user ID
         */
        std::string GetUserId();

        /**
         * @brief Returns the user password
         *
         * @return user password
         */
        std::string GetPassword();

        /**
         * @brief Returns the client secret from the AuthParameters
         *
         * @return client secret
         */
        std::string GetClientSecret();

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

#endif// AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_REQUEST_H
