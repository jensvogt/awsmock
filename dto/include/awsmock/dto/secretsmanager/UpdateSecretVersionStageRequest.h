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
     * @brief Move a staging label (e.g. AWSCURRENT) from one secret version to another.
     *
     * Example:
     * @code{.json}
     * {
     *   "SecretId": "string",
     *   "VersionStage": "string",
     *   "MoveToVersionId": "string",
     *   "RemoveFromVersionId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateSecretVersionStageRequest final : Common::BaseCounter<UpdateSecretVersionStageRequest> {

        /**
         * Secret ID: ARN or name
         */
        std::string secretId;

        /**
         * Staging label to move
         */
        std::string versionStage;

        /**
         * Version ID to move the staging label to
         */
        std::string moveToVersionId;

        /**
         * Version ID to remove the staging label from
         */
        std::string removeFromVersionId;

      private:

        friend UpdateSecretVersionStageRequest tag_invoke(boost::json::value_to_tag<UpdateSecretVersionStageRequest>, boost::json::value const &v) {
            UpdateSecretVersionStageRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.versionStage = Core::Json::GetStringValue(v, "VersionStage");
            r.moveToVersionId = Core::Json::GetStringValue(v, "MoveToVersionId");
            r.removeFromVersionId = Core::Json::GetStringValue(v, "RemoveFromVersionId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSecretVersionStageRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"VersionStage", obj.versionStage},
                    {"MoveToVersionId", obj.moveToVersionId},
                    {"RemoveFromVersionId", obj.removeFromVersionId},
            };
        }
    };
}// namespace Awsmock::Dto::SecretsManager
