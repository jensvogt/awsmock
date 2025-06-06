//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_ENCRYPT_RESPONSE_H
#define AWSMOCK_DTO_KMS_ENCRYPT_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Encryption of supplied blob response
     *
     * Example:
     * @code{.json}
     * {
     *   "CiphertextBlob": blob,
     *   "EncryptionAlgorithm": "string",
     *   "KeyId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EncryptResponse final : Common::BaseCounter<EncryptResponse> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Encryption algorithm
         */
        EncryptionAlgorithm encryptionAlgorithm;

        /**
         * Cipher text.
         *
         * <p>
         * The cipher text is Base64 encoded in case of AWS CLI, plain otherwise.
         * </p>
         */
        std::string ciphertext;

      private:

        friend EncryptResponse tag_invoke(boost::json::value_to_tag<EncryptResponse>, boost::json::value const &v) {
            EncryptResponse r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.ciphertext = Core::Json::GetStringValue(v, "CiphertextBlob");
            r.encryptionAlgorithm = EncryptionAlgorithmsFromString(Core::Json::GetStringValue(v, "EncryptionAlgorithm"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EncryptResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"CiphertextBlob", obj.ciphertext},
                    {"EncryptionAlgorithm", EncryptionAlgorithmsToString(obj.encryptionAlgorithm)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_ENCRYPT_RESPONSE_H
