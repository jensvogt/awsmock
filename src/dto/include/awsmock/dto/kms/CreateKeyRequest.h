//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_CREATE_KEY_REQUEST_H
#define AWSMOCK_DTO_KMS_CREATE_KEY_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include "awsmock/dto/common/BaseRequest.h"
#include "awsmock/dto/kms/model/KeySpec.h"
#include "awsmock/dto/kms/model/KeyUsage.h"
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::KMS {

    /**
     * Create KMS key request
     *
     * Example:
     * @code{.json}
     * {
     *   "BypassPolicyLockoutSafetyCheck": boolean,
     *   "KeySpec": "string",
     *   "CustomKeyStoreId": "string",
     *   "Description": "string",
     *   "KeySpec": "string",
     *   "KeyUsage": "string",
     *   "MultiRegion": boolean,
     *   "Origin": "string",
     *   "Policy": "string",
     *   "Tags": [
     *     {
     *       "TagKey": "string",
     *       "TagValue": "string"
     *     }
     *   ],
     *   "XksKeyId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateKeyRequest : public Dto::Common::BaseRequest {

        /**
         * Customer key store ID
         */
        std::string customKeyStoreId;

        /**
         * Description
         */
        std::string description;

        /**
         * Key specification
         */
        KeySpec keySpec = KeySpec::SYMMETRIC_DEFAULT;

        /**
         * Key usage
         */
        KeyUsage keyUsage = KeyUsage::ENCRYPT_DECRYPT;

        /**
         * Multi region
         */
        bool multiRegion = false;

        /**
         * Bypass policy lockout safety check
         */
        bool bypassPolicyLockoutSafetyCheck = false;

        /**
         * Origin
         */
        std::string origin;

        /**
         * Policy
         */
        std::string policy;

        /**
         * Xks key ID
         */
        std::string xksKeyId;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Converts the JSON string to DTO.
         *
         * @param jsonString JSON string
        */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const CreateKeyRequest &r);
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_CREATE_KEY_REQUEST_H
