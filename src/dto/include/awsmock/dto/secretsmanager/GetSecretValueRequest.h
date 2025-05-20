//
// Created by vogje01 on 4/9/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Get a secret value
     *
     * Example:
     * @code{.json}
     * {
     *    "SecretId": "string",
     *    "VersionId": "string",
     *    "VersionStage": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetSecretValueRequest final : Common::BaseCounter<GetSecretValueRequest> {

        /**
         * Secret ID
         */
        std::string secretId;

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * Version stage
         */
        std::string versionStage;

      private:

        friend GetSecretValueRequest tag_invoke(boost::json::value_to_tag<GetSecretValueRequest>, boost::json::value const &v) {
            GetSecretValueRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            r.versionStage = Core::Json::GetStringValue(v, "VersionStage");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetSecretValueRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"VersionId", obj.versionId},
                    {"VersionStage", obj.versionStage},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_REQUEST_H
