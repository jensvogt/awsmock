//
// Created by vogje01 on 12/18/23.
//

#pragma once

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::Cognito {

    /**
     * @brief Validation units
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum ValidityUnits {
        seconds,
        minutes,
        hours,
        days
    };

    static std::map<ValidityUnits, std::string> ValidityUnitsNames{
            {seconds, "seconds"},
            {minutes, "minutes"},
            {hours, "hours"},
            {days, "days"}};

    [[maybe_unused]] static std::string ValidityUnitToString(const ValidityUnits &validityUnits) {
        return ValidityUnitsNames[validityUnits];
    }

    [[maybe_unused]] static ValidityUnits ValidityUnitFromString(const std::string &validityUnits) {
        for (auto &[fst, snd]: ValidityUnitsNames) {
            if (snd == validityUnits) {
                return fst;
            }
        }
        return days;
    }

    struct TokenValidityUnits final : Common::BaseObject<TokenValidityUnits> {

        /**
         * Access token
         */
        ValidityUnits accessToken = hours;

        /**
         * ID token
         */
        ValidityUnits idToken = hours;

        /**
         * Refresh token
         */
        ValidityUnits refreshToken = hours;

      private:

        friend TokenValidityUnits tag_invoke(boost::json::value_to_tag<TokenValidityUnits>, boost::json::value const &v) {
            TokenValidityUnits r;
            r.accessToken = ValidityUnitFromString(Core::Json::GetStringValue(v, "AccessToken"));
            r.idToken = ValidityUnitFromString(Core::Json::GetStringValue(v, "IdToken"));
            r.refreshToken = ValidityUnitFromString(Core::Json::GetStringValue(v, "RefreshToken"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TokenValidityUnits const &obj) {
            jv = {
                    {"AccessToken", ValidityUnitToString(obj.accessToken)},
                    {"IdToken", ValidityUnitToString(obj.idToken)},
                    {"RefreshToken", ValidityUnitToString(obj.refreshToken)},
            };
        }
    };
}// namespace Awsmock::Dto::Cognito
