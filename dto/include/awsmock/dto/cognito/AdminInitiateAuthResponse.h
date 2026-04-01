//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_ADMIN_INITIATE_AUTH_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_ADMIN_INITIATE_AUTH_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/cognito/model/AuthenticationResult.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Admin initiate auth response
     * @code{.json}
     * {
     *   "AuthenticationResult": {
     *     "AccessToken": "string",
     *     "ExpiresIn": number,
     *     "IdToken": "string",
     *     "NewDeviceMetadata": {
     *        "DeviceGroupKey": "string",
     *        "DeviceKey": "string"
     *     },
     *     "RefreshToken": "string",
     *     "TokenType": "string"
     *   },
     *   "AvailableChallenges": [ "string" ],
     *   "ChallengeName": "string",
     *   "ChallengeParameters": {
     *     "string" : "string"
     *   },
     *   "Session": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AdminInitiateAuthResponse final : Common::BaseCounter<AdminCreateUserResponse> {

        /**
         * Authentication result
         */
        AuthenticationResult authenticationResult;

        /**
         * Available challenges
         */
        std::vector<std::string> availableChallenges;

        /**
         * Challenge name
         */
        std::string challengeName;

        /**
         * Challenge parameters
         */
        std::map<std::string, std::string> challengeParameters;

        /**
         * Session
         */
        std::string session;

      private:

        friend AdminInitiateAuthResponse tag_invoke(boost::json::value_to_tag<AdminInitiateAuthResponse>, boost::json::value const &v) {
            AdminInitiateAuthResponse r = {};
            r.challengeName = Core::Json::GetStringValue(v, "ChallengeName");
            r.session = Core::Json::GetStringValue(v, "Session");
            if (Core::Json::AttributeExists(v, "AuthenticationResult")) {
                r.authenticationResult = boost::json::value_to<AuthenticationResult>(v.at("AuthenticationResult"));
            }
            if (Core::Json::AttributeExists(v, "AvailableChallenges")) {
                r.availableChallenges = boost::json::value_to<std::vector<std::string>>(v.at("AvailableChallenges"));
            }
            if (Core::Json::AttributeExists(v, "ChallengeParameters")) {
                r.challengeParameters = boost::json::value_to<std::map<std::string, std::string>>(v.at("ChallengeParameters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AdminInitiateAuthResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"RequestId", obj.requestId},
                    {"ChallengeName", obj.challengeName},
                    {"Session", obj.session},
                    {"AuthenticationResult", boost::json::value_from(obj.authenticationResult)},
                    {"AvailableChallenges", boost::json::value_from(obj.availableChallenges)},
                    {"ChallengeParameters", boost::json::value_from(obj.challengeParameters)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_ADMIN_INITIATE_AUTH_RESPONSE_H
