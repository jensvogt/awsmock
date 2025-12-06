//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_REQUEST_H
#define AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/ChallengeName.h>
#include <awsmock/dto/cognito/model/UserContextData.h>
#include <awsmock/dto/common/BaseCounter.h>

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
    struct RespondToAuthChallengeRequest final : Common::BaseCounter<RespondToAuthChallengeRequest> {

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
        ChallengeName challengeName = ChallengeName::CUSTOM_CHALLENGE;

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
        std::string GetUserName() {
            return challengeResponses["USERNAME"];
        }

        /**
         * @brief Returns the user password
         *
         * @return user password
         */
        std::string GetPasswordClaim_Signature() {
            return challengeResponses["PASSWORD_CLAIM_SIGNATURE"];
        }

        /**
         * @brief Returns the client secret from the AuthParameters
         *
         * @return client secret
         */
        std::string GetPasswordClaimSecretBlock() {
            return challengeResponses["PASSWORD_CLAIM_SECRET_BLOCK"];
        }

      private:

        friend RespondToAuthChallengeRequest tag_invoke(boost::json::value_to_tag<RespondToAuthChallengeRequest>, boost::json::value const &v) {
            RespondToAuthChallengeRequest r;
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            r.session = Core::Json::GetStringValue(v, "Session");
            r.challengeName = ChallengeNameFromString(Core::Json::GetStringValue(v, "ChallengeName"));
            r.challengeResponses = boost::json::value_to<std::map<std::string, std::string>>(v.at("ChallengeResponses"));
            r.clientMetaData = boost::json::value_to<std::map<std::string, std::string>>(v.at("ChallengeResponses"));
            r.userContextData = boost::json::value_to<UserContextData>(v.at("UserContextData"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RespondToAuthChallengeRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ClientId", obj.clientId},
                    {"Session", obj.session},
                    {"ChallengeName", ChallengeNameToString(obj.challengeName)},
                    {"ChallengeResponses", boost::json::value_from(obj.challengeResponses)},
                    {"ClientMetaData", boost::json::value_from(obj.clientMetaData)},
                    {"UserContextData", boost::json::value_from(obj.userContextData)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_RESPOND_TO_AUTH_CHALLENGE_REQUEST_H
