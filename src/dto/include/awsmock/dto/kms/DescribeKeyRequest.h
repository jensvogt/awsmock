//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_DESCRIBE_KEY_REQUEST_H
#define AWSMOCK_DTO_KMS_DESCRIBE_KEY_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Create KMS key request
     *
     * Example:
     * @code{.json}
     * {
     *   "GrantTokens": [ "string" ],
     *   "KeyId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeKeyRequest : Common::BaseDto<DescribeKeyRequest> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Grant token
         */
        std::vector<std::string> grantTokens;

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

#endif// AWSMOCK_DTO_KMS_DESCRIBE_KEY_REQUEST_H
