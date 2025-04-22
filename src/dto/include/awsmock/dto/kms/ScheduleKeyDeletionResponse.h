//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_SCHEDULE_KEY_DELETION_RESPONSE_H
#define AWSMOCK_DTO_KMS_SCHEDULE_KEY_DELETION_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief Create KMS key response
     *
     * Example:
     * @code{.json}
     * {
     *   "DeletionDate": number,
     *   "KeyId": "string",
     *   "KeyState": "string",
     *   "PendingWindowInDays": number
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ScheduledKeyDeletionResponse : Common::BaseDto<ScheduledKeyDeletionResponse> {

        /**
         * Deletion date as timestamp
         */
        std::string keyId;

        /**
         * Key state
         */
        std::string keyState;

        /**
         * Deletion date as timestamp
         */
        long deletionDate;

        /**
         * Pending window in days
         */
        int pendingWindowInDays;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_SCHEDULE_KEY_DELETION_RESPONSE_H
