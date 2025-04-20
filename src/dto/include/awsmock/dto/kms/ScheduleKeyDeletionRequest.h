//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_SCHEDULE_KEX_DELETION_REQUEST_H
#define AWSMOCK_DTO_KMS_SCHEDULE_KEX_DELETION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Schedule a KMS key deletion
     *
     * Example:
     * @code{.json}
     * {
     *   "KeyId": "string",
     *   "PendingWindowInDays": number
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ScheduleKeyDeletionRequest : Common::BaseRequest<ScheduleKeyDeletionRequest> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Pending window in days
         */
        int pendingWindowInDays = 30;

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

#endif// AWSMOCK_DTO_KMS_SCHEDULE_KEX_DELETION_REQUEST_H
