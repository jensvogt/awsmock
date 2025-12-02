//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H
#define AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
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
    struct CreateKeyResponse final : Common::BaseCounter<CreateKeyResponse> {

        /**
         * Key metadata
         */
        Key key;

      private:

        friend CreateKeyResponse tag_invoke(boost::json::value_to_tag<CreateKeyResponse>, boost::json::value const &v) {
            CreateKeyResponse r;
            r.key = boost::json::value_to<Key>(v.at("Key"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateKeyResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Key", boost::json::value_from(obj.key)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_CREATE_KEY_RESPONSE_H
