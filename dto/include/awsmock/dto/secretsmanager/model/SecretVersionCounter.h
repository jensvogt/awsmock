//
// Created by vogje01 on 4/11/24.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>
#include <awsmock/dto/secretsmanager/model/RotationRules.h>

namespace Awsmock::Dto::SecretsManager {

    /**
     * @brief Secret version counter
     *
     * Example:
     * @code{.json}
     *  {
     *     "VersionId": "string",
     *     "States": [
     *          "string",
     *          ...
     *     ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SecretVersionCounter final : Common::BaseObject<SecretVersionCounter> {

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * States
         */
        std::vector<std::string> states;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Last accessed
         */
        system_clock::time_point lastAccessed;

      private:

        friend SecretVersionCounter tag_invoke(boost::json::value_to_tag<SecretVersionCounter>, boost::json::value const &v) {
            SecretVersionCounter r;
            r.versionId = Core::Json::GetStringValue(v, "secretName");
            r.states = boost::json::value_to<std::vector<std::string>>(v.at("states"));
            r.created = Core::Json::GetDatetimeValue(v, "created");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SecretVersionCounter const &obj) {
            jv = {
                    {"versionId", obj.versionId},
                    {"states", boost::json::value_from(obj.states)},
            };
        }
    };
}// namespace Awsmock::Dto::SecretsManager
