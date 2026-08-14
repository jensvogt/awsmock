//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SecretsManager {

    /**
     * @brief Generate a random password.
     *
     * Example:
     * @code{.json}
     * {
     *   "ExcludeCharacters": "string",
     *   "ExcludeLowercase": boolean,
     *   "ExcludeNumbers": boolean,
     *   "ExcludePunctuation": boolean,
     *   "ExcludeUppercase": boolean,
     *   "IncludeSpace": boolean,
     *   "PasswordLength": number,
     *   "RequireEachIncludedType": boolean
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetRandomPasswordRequest final : Common::BaseCounter<GetRandomPasswordRequest> {

        /**
         * Requested password length
         */
        long passwordLength = 32;

      private:

        friend GetRandomPasswordRequest tag_invoke(boost::json::value_to_tag<GetRandomPasswordRequest>, boost::json::value const &v) {
            GetRandomPasswordRequest r;
            if (Core::Json::AttributeExists(v, "PasswordLength")) {
                r.passwordLength = Core::Json::GetLongValue(v, "PasswordLength");
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetRandomPasswordRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"PasswordLength", obj.passwordLength},
            };
        }
    };
}// namespace Awsmock::Dto::SecretsManager
