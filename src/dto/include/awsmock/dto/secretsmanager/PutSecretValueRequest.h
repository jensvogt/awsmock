//
// Created by vogje01 on 4/9/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_PUT_SECRET_VALUE_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_PUT_SECRET_VALUE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Put a secret value
     *
     * @par
     * Creates a new version of your secret by creating a new encrypted value and attaching it to the secret. version can contain a new SecretString value or a new SecretBinary value.
     *
     * Example:
     * @code{.json}
     * {
     *   "ClientRequestToken": "string",
     *   "RotationToken": "string",
     *   "SecretBinary": blob,
     *   "SecretId": "string",
     *   "SecretString": "string",
     *   "VersionStages": [ "string" ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutSecretValueRequest final : Common::BaseCounter<PutSecretValueRequest> {

        /**
         * Secret ID
         */
        std::string secretId;

        /**
         * Rotation token
         */
        std::string rotationToken;

        /**
         * Client token
         */
        std::string clientRequestToken;

        /**
         * Secret string
         */
        std::string secretString;

        /**
         * Secret binary
         */
        std::string secretBinary;

        /**
         * Version stage
         */
        std::vector<std::string> versionStage;

      private:

        friend PutSecretValueRequest tag_invoke(boost::json::value_to_tag<PutSecretValueRequest>, boost::json::value const &v) {
            PutSecretValueRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.rotationToken = Core::Json::GetStringValue(v, "RotationToken");
            r.clientRequestToken = Core::Json::GetStringValue(v, "ClientRequestToken");
            r.secretString = Core::Json::GetStringValue(v, "SecretString");
            r.secretBinary = Core::Json::GetStringValue(v, "SecretBinary");
            if (Core::Json::AttributeExists(v, "VersionStage")) {
                r.versionStage = boost::json::value_to<std::vector<std::string>>(v.at("VersionStage"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutSecretValueRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"RotationToken", obj.rotationToken},
                    {"ClientRequestToken", obj.clientRequestToken},
                    {"SecretString", obj.secretString},
                    {"SecretBinary", obj.secretBinary},
                    {"VersionStage", boost::json::value_from(obj.versionStage)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_REQUEST_H
