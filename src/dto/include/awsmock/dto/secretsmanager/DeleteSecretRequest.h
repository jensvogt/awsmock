//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_DELETE_SECRET_REQUEST_H
#define AWSMOCK_CORE_DTO_DELETE_SECRET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Delete secret request
     *
     * Example:
     * @code{.json}
     * {
     *   "ForceDeleteWithoutRecovery": boolean,
     *   "RecoveryWindowInDays": number,
     *   "SecretId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteSecretRequest final : Common::BaseCounter<DeleteSecretRequest> {

        /**
         * Secret ID; can be the name of the secret or ARN
         */
        std::string secretId;

        /**
         * Force deletion
         */
        bool forceDeleteWithoutRecovery = false;

        /**
         * Recovery windows in days
         */
        long recoveryWindowInDays{};

      private:

        friend DeleteSecretRequest tag_invoke(boost::json::value_to_tag<DeleteSecretRequest>, boost::json::value const &v) {
            DeleteSecretRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.forceDeleteWithoutRecovery = Core::Json::GetBoolValue(v, "ForceDeleteWithoutRecovery");
            r.recoveryWindowInDays = Core::Json::GetLongValue(v, "RecoveryWindowInDays");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteSecretRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"ForceDeleteWithoutRecovery", obj.forceDeleteWithoutRecovery},
                    {"RecoveryWindowInDays", obj.recoveryWindowInDays},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_CORE_DTO_DELETE_SECRET_REQUEST_H
