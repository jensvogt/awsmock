//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_ENCRYPT_RESPONSE_H
#define AWSMOCK_DTO_KMS_ENCRYPT_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>
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
    struct EncryptResponse : Common::BaseRequest {

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

        /**
         * @brief Converts the JSON string to DTO.
         *
         * @param jsonString JSON string
        */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const EncryptResponse &r);
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_ENCRYPT_RESPONSE_H
