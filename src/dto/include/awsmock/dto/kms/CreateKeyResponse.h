//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H
#define AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/kms/model/Key.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Create KMS key response
     *
     * Example:
     * @code{.json}
     * {
     *   "Key": {
     *      "Arn": "string",
     *      "AWSAccountId": "string",
     *      "CloudHsmClusterId": "string",
     *      "CreationDate": number,
     *      "KeySpec": "string",
     *      "CustomKeyStoreId": "string",
     *      "DeletionDate": number,
     *      "Description": "string",
     *      "Enabled": boolean,
     *      "EncryptionAlgorithms": [ "string" ],
     *      "ExpirationModel": "string",
     *      "KeyId": "string",
     *      "KeyManager": "string",
     *      "KeySpec": "string",
     *      "KeyState": "string",
     *      "KeyUsage": "string",
     *      "MacAlgorithms": [ "string" ],
     *      "MultiRegion": boolean,
     *      "MultiRegionConfiguration": {
     *         "MultiRegionKeyType": "string",
     *         "PrimaryKey": {
     *            "Arn": "string",
     *            "Region": "string"
     *         },
     *         "ReplicaKeys": [
     *           {
     *             "Arn": "string",
     *             "Region": "string"
     *           }
     *         ]
     *      },
     *      "Origin": "string",
     *      "PendingDeletionWindowInDays": number,
     *      "SigningAlgorithms": [ "string" ],
     *      "ValidTo": number,
     *      "XksKeyConfiguration": {
     *         "Id": "string"
     *      }
     *    }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateKeyResponse final : Common::BaseDto<CreateKeyResponse> {

        /**
         * Key metadata
         */
        Key key;

        /**
         * @brief Convert to from a JSON string
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

#endif// AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H
