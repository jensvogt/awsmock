//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_DECRYPT_REQUEST_H
#define AWSMOCK_DTO_KMS_DECRYPT_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Decryption request
     *
     * <p>
     * Recipients are not implemented yet.
     * </p>
     *
     * Example:
     * @code{.json}
     * {
     *   "CiphertextBlob": blob,
     *   "DryRun": boolean,
     *   "EncryptionAlgorithm": "string",
     *   "EncryptionContext": {
     *     "string" : "string"
     *   },
     *   "GrantTokens": [ "string" ],
     *   "KeyId": "string",
     *   "Recipient": {
     *     "AttestationDocument": blob,
     *     "KeyEncryptionAlgorithm": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DecryptRequest final : Common::BaseCounter<DecryptRequest> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Plain text
         *
         * <p>
         * The plain text is Base64 encoded in case of AWS CLI, plain otherwise.
         * </p>
         */
        std::string ciphertext;

        /**
         * Encryption algorithm
         */
        EncryptionAlgorithm encryptionAlgorithm = EncryptionAlgorithm::UNKNOWN;

        /**
         * Encryption model
         */
        std::map<std::string, std::string> encryptionContext;

        /**
         * Grant tokens
         */
        std::vector<std::string> grantTokens;

        /**
         * Dry run
         */
        bool dryRun = false;

      private:

        friend DecryptRequest tag_invoke(boost::json::value_to_tag<DecryptRequest>, boost::json::value const &v) {
            DecryptRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.ciphertext = Core::Json::GetStringValue(v, "CiphertextBlob");
            r.encryptionAlgorithm = EncryptionAlgorithmsFromString(Core::Json::GetStringValue(v, "EncryptionAlgorithm"));
            r.dryRun = Core::Json::GetBoolValue(v, "DryRun");
            if (Core::Json::AttributeExists(v, "GrantTokens")) {
                r.grantTokens = boost::json::value_to<std::vector<std::string>>(v.at("GrantTokens"));
            }
            if (Core::Json::AttributeExists(v, "EncryptionContext")) {
                r.encryptionContext = boost::json::value_to<std::map<std::string, std::string>>(v.at("EncryptionContext"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DecryptRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"CiphertextBlob", obj.ciphertext},
                    {"EncryptionAlgorithm", EncryptionAlgorithmsToString(obj.encryptionAlgorithm)},
                    {"EncryptionContext", boost::json::value_from(obj.encryptionContext)},
                    {"GrantTokens", boost::json::value_from(obj.grantTokens)},
                    {"DryRun", obj.dryRun},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_DECRYPT_REQUEST_H
