//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEYS_REQUEST_H
#define AWSMOCK_DTO_KMS_LIST_KEYS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief List all KMS keys request
     *
     * Example:
     * @code{.json}
     * {
     *   "Limit": number,
     *   "Marker": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKeysRequest : Common::BaseDto<ListKeysRequest> {


        /**
         * Limit
         */
        long limit;

        /**
         * Marker for paging
         */
        std::string marker;

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

#endif// AWSMOCK_DTO_KMS_LIST_KEYS_REQUEST_H
