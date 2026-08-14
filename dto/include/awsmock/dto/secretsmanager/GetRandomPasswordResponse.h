//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SecretsManager {

    /**
     * @brief Generate a random password response.
     *
     * Example:
     * @code{.json}
     * {
     *   "RandomPassword": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetRandomPasswordResponse final : Common::BaseCounter<GetRandomPasswordResponse> {

        /**
         * Generated password
         */
        std::string randomPassword;

      private:

        friend GetRandomPasswordResponse tag_invoke(boost::json::value_to_tag<GetRandomPasswordResponse>, boost::json::value const &v) {
            GetRandomPasswordResponse r;
            r.randomPassword = Core::Json::GetStringValue(v, "RandomPassword");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetRandomPasswordResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"RandomPassword", obj.randomPassword},
            };
        }
    };
}// namespace Awsmock::Dto::SecretsManager
