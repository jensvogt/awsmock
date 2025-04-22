//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_DESCRIBE_KEY_RESPONSE_H
#define AWSMOCK_DTO_KMS_DESCRIBE_KEY_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/kms/model/Key.h>

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
    struct DescribeKeyResponse final : Common::BaseDto<DescribeKeyResponse> {

        /**
         * Key metadata
         */
        Key key;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_DESCRIBE_KEY_RESPONSE_H
