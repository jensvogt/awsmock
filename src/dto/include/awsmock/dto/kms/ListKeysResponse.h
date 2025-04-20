//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEYS_RESPONSE_H
#define AWSMOCK_DTO_KMS_LIST_KEYS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>
#include <awsmock/dto/kms/model/ListKey.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief List all KMS keys request
     *
     * Example:
     * @code{.json}
     * {
     *   "Keys": [
     *     {
     *       "KeyArn": "string",
     *       "KeyId": "string"
     *     }
     *   ],
     *  "NextMarker": "string",
     *  "Truncated": boolean
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKeysResponse : Common::BaseRequest<ListKeysResponse> {

        /**
         * Key
         */
        std::vector<ListKey> keys;

        /**
         * Next marker for paging
         */
        std::string nextMarker;

        /**
         * Truncated
         */
        bool truncated = false;

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
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_LIST_KEYS_RESPONSE_H
