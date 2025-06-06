//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_INITIATE_AUTH_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_INITIATE_AUTH_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/AuthenticationResult.h>
#include <awsmock/dto/common/BaseCounter.h>

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
    struct InitiateAuthResponse final : Common::BaseCounter<InitiateAuthResponse> {

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

      private:

        friend InitiateAuthResponse tag_invoke(boost::json::value_to_tag<InitiateAuthResponse>, boost::json::value const &v) {
            InitiateAuthResponse r;
            r.authenticationResult = boost::json::value_to<AuthenticationResult>(v.at("AuthenticationResult"));
            r.availableChallenges = boost::json::value_to<std::vector<std::string>>(v.at("AvailableChallenges"));
            r.session = Core::Json::GetStringValue(v, "Session");
            r.clientId = Core::Json::GetStringValue(v, "ClientId");
            r.challengeName = Core::Json::GetStringValue(v, "ChallengeName");
            r.challengeParameters = boost::json::value_to<std::map<std::string, std::string>>(v.at("ChallengeParameters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, InitiateAuthResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"AuthenticationResult", boost::json::value_from(obj.authenticationResult)},
                    {"AvailableChallenges", boost::json::value_from(obj.availableChallenges)},
                    {"Session", obj.session},
                    {"ClientId", obj.clientId},
                    {"ChallengeName", obj.challengeName},
                    {"ChallengeParameters", boost::json::value_from(obj.challengeParameters)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_INITIATE_AUTH_RESPONSE_H
