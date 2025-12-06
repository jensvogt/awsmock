//
// Created by vogje01 on 4/11/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_SECRET_VERSION_H
#define AWSMOCK_DTO_SECRETSMANAGER_SECRET_VERSION_H


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
    struct SecretVersion final : Common::BaseCounter<SecretVersion> {

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
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"VersionId", obj.versionId},
                    {"KmsKeyIds", boost::json::value_from(obj.kmsKeyIds)},
                    {"VersionStages", boost::json::value_from(obj.versionStages)},
                    {"CreatedDate", Core::DateTimeUtils::UnixTimestamp(obj.created)},
                    {"LastAccessedDate", Core::DateTimeUtils::UnixTimestamp(obj.lastAccessed)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_SECRET_VERSION_H
