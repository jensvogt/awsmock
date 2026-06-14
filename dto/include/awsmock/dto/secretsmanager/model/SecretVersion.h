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
    struct SecretVersion final : Common::BaseObject<SecretVersion> {

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * KMS key ID
         */
        std::vector<std::string> kmsKeyIds;

        /**
         * Stages
         */
        std::vector<std::string> versionStages;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Last accessed
         */
        system_clock::time_point lastAccessed;

      private:

        friend SecretVersion tag_invoke(boost::json::value_to_tag<SecretVersion>, boost::json::value const &v) {
            SecretVersion r;
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            r.kmsKeyIds = boost::json::value_to<std::vector<std::string>>(v.at("KmsKeyIds"));
            r.versionStages = boost::json::value_to<std::vector<std::string>>(v.at("VersionStages"));
            r.created = Core::Json::GetDatetimeValue(v, "CreatedDate");
            r.lastAccessed = Core::Json::GetDatetimeValue(v, "LastAccessedDate");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SecretVersion const &obj) {
            jv = {
                    {"VersionId", obj.versionId},
                    {"KmsKeyIds", boost::json::value_from(obj.kmsKeyIds)},
                    {"VersionStages", boost::json::value_from(obj.versionStages)},
                    {"CreatedDate", Core::DateTimeUtils::UnixTimestamp(obj.created)},
                    {"LastAccessedDate", Core::DateTimeUtils::UnixTimestamp(obj.lastAccessed)},
            };
        }
    };

}// namespace Awsmock::Dto::SecretsManager
