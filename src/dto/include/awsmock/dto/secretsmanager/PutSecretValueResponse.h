//
// Created by vogje01 on 4/9/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_PUT_SECRET_VALUE_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_PUT_SECRET_VALUE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Put a secret value response
     *
     * Example:
     * @code{.json}
     * {
     *   "ARN": "string",
     *   "Name": "string",
     *   "VersionId": "string",
     *   "VersionStages": [ "string" ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutSecretValueResponse final : Common::BaseCounter<PutSecretValueResponse> {

        /*+
         * Name
         */
        std::string name;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * Version ID
         */
        std::vector<std::string> versionStages;

      private:

        friend PutSecretValueResponse tag_invoke(boost::json::value_to_tag<PutSecretValueResponse>, boost::json::value const &v) {
            PutSecretValueResponse r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            r.versionStages = boost::json::value_to<std::vector<std::string>>(v.at("VersionId"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutSecretValueResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ARN", obj.arn},
                    {"VersionId", obj.versionId},
                    {"VersionStages", boost::json::value_from(obj.versionStages)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_PUT_SECRET_VALUE_RESPONSE_H
