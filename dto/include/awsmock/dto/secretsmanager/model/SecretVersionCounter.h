//
// Created by vogje01 on 4/11/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_SECRET_VERSION_COUNTER_H
#define AWSMOCK_DTO_SECRETSMANAGER_SECRET_VERSION_COUNTER_H


// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/RotationRules.h>

namespace AwsMock::Dto::SecretsManager {

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
    struct SecretVersionCounter final : Common::BaseCounter<SecretVersionCounter> {

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
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"versionId", obj.versionId},
                    {"states", boost::json::value_from(obj.states)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_SECRET_VERSION_COUNTER_H
