//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * Update secret response.
     *
     * Syntax:
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
    struct UpdateSecretResponse final : Common::BaseCounter<UpdateSecretResponse> {

        /**
         * Name
         */
        std::string name;

        /**
         * Secret ARN
         */
        std::string arn;

        /**
         * VersionId
         */
        std::string versionId;

      private:

        friend UpdateSecretResponse tag_invoke(boost::json::value_to_tag<UpdateSecretResponse>, boost::json::value const &v) {
            UpdateSecretResponse r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSecretResponse const &obj) {
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

#endif// AWSMOCK_DTO_SECRETSMANAGER_UPDATE_SECRET_RESPONSE_H
