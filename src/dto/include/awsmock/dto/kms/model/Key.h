//
// Created by vogje01 on 4/26/24.
//

#ifndef AWSMOCK_DTO_KMS_KEY_H
#define AWSMOCK_DTO_KMS_KEY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/kms/model/EncryptionAlgorithm.h>
#include <awsmock/dto/kms/model/KeySpec.h>
#include <awsmock/dto/kms/model/KeyState.h>
#include <awsmock/dto/kms/model/KeyUsage.h>
#include <awsmock/dto/kms/model/Origin.h>

namespace AwsMock::Dto::KMS {

    /**
     * KMS metadata
     *
     * Example:
     * @code{.json}
     * {
     *   "Arn": "string",
     *   "AWSAccountId": "string",
     *   "CloudHsmClusterId": "string",
     *   "CreationDate": number,
     *   "CustomerMasterKeySpec": "string",
     *   "CustomKeyStoreId": "string",
     *   "DeletionDate": number,
     *   "Description": "string",
     *   "Enabled": boolean,
     *   "EncryptionAlgorithms": [ "string" ],
     *   "ExpirationModel": "string",
     *   "KeyId": "string",
     *   "KeyManager": "string",
     *   "KeySpec": "string",
     *   "KeyState": "string",
     *   "KeyUsage": "string",
     *   "MacAlgorithms": [ "string" ],
     *   "MultiRegion": boolean,
     *   "MultiRegionConfiguration": {
     *     "MultiRegionKeyType": "string",
     *     "PrimaryKey": {
     *       "Arn": "string",
     *       "Region": "string"
     *     },
     *     "ReplicaKeys": [
     *       {
     *         "Arn": "string",
     *         "Region": "string"
     *       }
     *     ]
     *   },
     *   "Origin": "string",
     *   "PendingDeletionWindowInDays": number,
     *   "SigningAlgorithms": [ "string" ],
     *   "ValidTo": number,
     *   "XksKeyConfiguration": {
     *     "Id": "string"
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Key {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Arn
         */
        std::string arn;

        /**
         * Key specification
         */
        KeySpec keySpec;

        /**
         * Key usage
         */
        KeyUsage keyUsage;

        /**
         * Key state
         */
        KeyState keyState;

        /**
         * Multi region
         */
        bool multiRegion;

        /**
         * Description
         */
        std::string description;

        /**
         * Creation date
         */
        long creationDate = system_clock::to_time_t(system_clock::now());

        /**
         * Deletion date
         */
        long deletionDate = 0;

        /**
         * Enabled flag
         */
        bool enabled;

        /**
         * Origin
         */
        Origin origin;

        /**
         * Encryption algorithms
         */
        std::vector<EncryptionAlgorithm> encryptionAlgorithms;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert to from a JSON object
         *
         * @param jsonObject JSON object
         */
        void FromDocument(const view_or_value<view, value> &jsonObject);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Key &r);
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_KEY_H
