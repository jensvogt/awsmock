//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_CREATE_SECRET_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_CREATE_SECRET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secrets manager create secret request
     *
     * Example:
     * @code{.json}
     * {
     *   "Name": "test",
     *   "ClientRequestToken": "8b0e8777-4c9a-4621-9bd7-a6449b24c3a5"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateSecretRequest final : Common::BaseCounter<CreateSecretRequest> {

        /**
         * Secret name
         */
        std::string name;

        /**
         * Client request token
         */
        std::string clientRequestToken;

        /**
         * Description
         */
        std::string description;

        /**
         * Secret string
         */
        std::string secretString;

        /**
         * Base64 encoded secret binary data
         */
        std::string secretBinary;

        /**
         * Force overwrite flag
         */
        bool forceOverwriteReplicaSecret{};

        /**
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend CreateSecretRequest tag_invoke(boost::json::value_to_tag<CreateSecretRequest>, boost::json::value const &v) {
            CreateSecretRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.clientRequestToken = Core::Json::GetStringValue(v, "ClientRequestToken");
            r.secretString = Core::Json::GetStringValue(v, "SecretString");
            r.secretBinary = Core::Json::GetStringValue(v, "SecretBinary");
            r.forceOverwriteReplicaSecret = Core::Json::GetBoolValue(v, "ForceOverwriteReplicaSecret");
            r.kmsKeyId = Core::Json::GetStringValue(v, "KmsKeyId");
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateSecretRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ClientRequestToken", obj.clientRequestToken},
                    {"SecretString", obj.secretString},
                    {"SecretBinary", obj.secretBinary},
                    {"ForceOverwriteReplicaSecret", obj.forceOverwriteReplicaSecret},
                    {"KmsKeyId", obj.kmsKeyId},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_CREATE_SECRET_REQUEST_H
