//
// Created by vogje01 on 6/5/24.
//

#ifndef AWSMOCK_DTO_COGNITO_CHALLENGE_NAME_H
#define AWSMOCK_DTO_COGNITO_CHALLENGE_NAME_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief Cognito auth flow enum
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ChallengeName {
        SMS_MFA,
        EMAIL_OTP,
        PASSWORD_VERIFIER,
        CUSTOM_CHALLENGE,
        DEVICE_SPR_AUTH,
        DEVICE_PASSWORD_VERIFIER,
        MFA_SETUP,
    };

    static std::map<ChallengeName, std::string> ChallengeNameNames{
            {ChallengeName::SMS_MFA, "SMS_MFA"},
            {ChallengeName::EMAIL_OTP, "EMAIL_OTP"},
            {ChallengeName::PASSWORD_VERIFIER, "PASSWORD_VERIFIER"},
            {ChallengeName::CUSTOM_CHALLENGE, "CUSTOM_CHALLENGE"},
            {ChallengeName::DEVICE_SPR_AUTH, "DEVICE_SPR_AUTH"},
            {ChallengeName::DEVICE_PASSWORD_VERIFIER, "DEVICE_PASSWORD_VERIFIER"},
            {ChallengeName::MFA_SETUP, "MFA_SETUP"},
    };

    [[maybe_unused]] static std::string ChallengeNameToString(const ChallengeName &challengeName) {
        return ChallengeNameNames[challengeName];
    }

    [[maybe_unused]] static ChallengeName ChallengeNameFromString(const std::string &challengeName) {
        for (auto &[fst, snd]: ChallengeNameNames) {
            if (snd == challengeName) {
                return fst;
            }
        }
        return ChallengeName::CUSTOM_CHALLENGE;
    }

}// namespace AwsMock::Dto::Cognito

#endif//AWSMOCK_DTO_COGNITO_CHALLENGE_NAME_H
