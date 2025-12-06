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
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

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

    struct TokenValidityUnits final : Common::BaseCounter<TokenValidityUnits> {

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
         * @brief Convert from a JSON object
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                accessToken = ValidityUnitFromString(Core::Bson::BsonUtils::GetStringValue(document, "AccessToken"));
                idToken = ValidityUnitFromString(Core::Bson::BsonUtils::GetStringValue(document, "IdToken"));
                refreshToken = ValidityUnitFromString(Core::Bson::BsonUtils::GetStringValue(document, "RefreshToken"));

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         * @throws JsonException
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "AccessToken", ValidityUnitToString(accessToken));
                Core::Bson::BsonUtils::SetStringValue(document, "IdToken", ValidityUnitToString(idToken));
                Core::Bson::BsonUtils::SetStringValue(document, "RefreshToken", ValidityUnitToString(refreshToken));
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend TokenValidityUnits tag_invoke(boost::json::value_to_tag<TokenValidityUnits>, boost::json::value const &v) {
            TokenValidityUnits r;
            r.accessToken = ValidityUnitFromString(Core::Json::GetStringValue(v, "accessToken"));
            r.idToken = ValidityUnitFromString(Core::Json::GetStringValue(v, "idToken"));
            r.refreshToken = ValidityUnitFromString(Core::Json::GetStringValue(v, "refreshToken"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TokenValidityUnits const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"accessToken", ValidityUnitToString(obj.accessToken)},
                    {"idToken", ValidityUnitToString(obj.idToken)},
                    {"refreshToken", ValidityUnitToString(obj.refreshToken)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_TOKEN_VALIDITY_UNITS_H
