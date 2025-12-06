//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_DECRYPT_RESPONSE_H
#define AWSMOCK_DTO_KMS_DECRYPT_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Decryption response
     *
     * <p>
     * Ciphertext recipients are not implemented yet.
     * </p>
     *
     * Example:
     * @code{.json}
     * {
     *   "CiphertextForRecipient": blob,
     *   "EncryptionAlgorithm": "string",
     *   "KeyId": "string",
     *   "Plaintext": blob
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DecryptResponse final : Common::BaseCounter<DecryptResponse> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Encryption algorithm
         */
        EncryptionAlgorithm encryptionAlgorithm = EncryptionAlgorithm::UNKNOWN;

        /**
         * @brief Plain text.
         *
         * <p>
         * The cipher text is Base64 encoded in case of AWS CLI, plain otherwise.
         * </p>
         */
        std::string plaintext;

      private:

        friend DecryptResponse tag_invoke(boost::json::value_to_tag<DecryptResponse>, boost::json::value const &v) {
            DecryptResponse r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.plaintext = Core::Json::GetStringValue(v, "Plaintext");
            r.encryptionAlgorithm = EncryptionAlgorithmsFromString(Core::Json::GetStringValue(v, "EncryptionAlgorithm"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DecryptResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"Plaintext", obj.plaintext},
                    {"EncryptionAlgorithm", EncryptionAlgorithmsToString(obj.encryptionAlgorithm)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_DECRYPT_RESPONSE_H
