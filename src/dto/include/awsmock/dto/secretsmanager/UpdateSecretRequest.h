//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_UPDATE_SECRET_REQUEST_H
#define AWSMOCK_CORE_DTO_UPDATE_SECRET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Update the secret values request.
     *
     * Example:
     * @code{.json}
     * {
     *   "ClientRequestToken": "string",
     *   "Description": "string",
     *   "KmsKeyId": "string",
     *   "SecretBinary": blob,
     *   "SecretId": "string",
     *   "SecretString": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateSecretRequest final : Common::BaseCounter<UpdateSecretRequest> {

        /**
         * Secret ID
         */
        std::string secretId;

        /**
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
         * Description
         */
        std::string description;

        /**
         * Secret string
         */
        std::string secretString;

        /**
         * Secret binary
         */
        std::string secretBinary;

      private:

        friend UpdateSecretRequest tag_invoke(boost::json::value_to_tag<UpdateSecretRequest>, boost::json::value const &v) {
            UpdateSecretRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.kmsKeyId = Core::Json::GetStringValue(v, "KmsKeyId");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.secretString = Core::Json::GetStringValue(v, "SecretString");
            r.secretBinary = Core::Json::GetStringValue(v, "SecretBinary");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateSecretRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"KmsKeyId", obj.kmsKeyId},
                    {"Description", obj.description},
                    {"SecretString", obj.secretString},
                    {"SecretBinary", obj.secretBinary},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_CORE_DTO_UPDATE_SECRET_REQUEST_H
