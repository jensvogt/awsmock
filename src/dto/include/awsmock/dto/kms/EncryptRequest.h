//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_ENCRYPT_REQUEST_H
#define AWSMOCK_DTO_KMS_ENCRYPT_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Encryption of supplied blob request
     *
     * Example:
     * @code{.json}
     * {
     *   "DryRun": boolean,
     *   "EncryptionAlgorithm": "string",
     *   "EncryptionContext": {
     *     "string" : "string"
     *   },
     *   "GrantTokens": [ "string" ],
     *   "KeyId": "string",
     *   "Plaintext": blob
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EncryptRequest : Common::BaseRequest {

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
        std::string plainText;

        /**
         * Encryption algorithm
         */
        EncryptionAlgorithm encryptionAlgorithm;

        /**
         * Grant tokens
         */
        std::vector<std::string> grantTokens;

        /**
         * Dry run
         */
        bool dryRun = false;

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
        friend std::ostream &operator<<(std::ostream &os, const EncryptRequest &r);
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_ENCRYPT_REQUEST_H
