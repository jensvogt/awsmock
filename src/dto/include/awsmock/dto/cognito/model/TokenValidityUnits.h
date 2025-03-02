//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_TOKEN_VALIDITY_UNITS_H
#define AWSMOCK_DTO_COGNITO_MODEL_TOKEN_VALIDITY_UNITS_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

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

    [[maybe_unused]] static std::string ValidityUnitToString(ValidityUnits validityUnits) {
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

    struct TokenValidityUnits {

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

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert from a JSON object
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         * @throws JsonException
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const TokenValidityUnits &r);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_TOKEN_VALIDITY_UNITS_H
