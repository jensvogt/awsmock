//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_ROTATE_SECRET_RESPONSE_H
#define AWSMOCK_CORE_DTO_ROTATE_SECRET_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Rotate a secret value response.
     *
     * Example:
     * @code{.json}
     * {
     *   "ARN": "string",
     *   "Name": "string",
     *   "VersionId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RotateSecretResponse final : Common::BaseCounter<RotateSecretResponse> {

        /**
         * Secret name
         */
        std::string name;

        /**
         * Secret ARN
         */
        std::string arn;

        /**
         * Version ID
         */
        std::string versionId;

      private:

        friend RotateSecretResponse tag_invoke(boost::json::value_to_tag<RotateSecretResponse>, boost::json::value const &v) {
            RotateSecretResponse r;
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.name = Core::Json::GetStringValue(v, "Name");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RotateSecretResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ARN", obj.arn},
                    {"VersionId", obj.versionId},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_CORE_DTO_ROTATE_SECRET_RESPONSE_H
