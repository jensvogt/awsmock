//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_DECRYPT_RESPONSE_H
#define AWSMOCK_DTO_KMS_DECRYPT_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/common/BaseDto.h>
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
    struct DecryptResponse : Common::BaseDto<DecryptResponse> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Encryption algorithm
         */
        EncryptionAlgorithm encryptionAlgorithm;

        /**
         * @brief Plain text.
         *
         * <p>
         * The cipher text is Base64 encoded in case of AWS CLI, plain otherwise.
         * </p>
         */
        std::string plaintext;

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
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_DECRYPT_RESPONSE_H
