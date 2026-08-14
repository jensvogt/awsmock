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
     * @brief Move a staging label response.
     *
     * Example:
     * @code{.json}
     * {
     *   "ARN": "string",
     *   "Name": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateSecretVersionStageResponse final : Common::BaseCounter<UpdateSecretVersionStageResponse> {

        /**
         * Secret name
         */
        std::string name;

        /**
         * Secret ARN
         */
        std::string arn;

      private:

        friend UpdateSecretVersionStageResponse tag_invoke(boost::json::value_to_tag<UpdateSecretVersionStageResponse>, boost::json::value const &v) {
            UpdateSecretVersionStageResponse r;
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.name = Core::Json::GetStringValue(v, "Name");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSecretVersionStageResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ARN", obj.arn},
            };
        }
    };
}// namespace Awsmock::Dto::SecretsManager
