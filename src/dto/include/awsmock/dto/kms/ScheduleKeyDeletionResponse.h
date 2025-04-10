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
#include <awsmock/dto/common/BaseRequest.h>

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
    struct ScheduledKeyDeletionResponse : Common::BaseRequest {

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
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ScheduledKeyDeletionResponse &r);
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_SCHEDULE_KEY_DELETION_RESPONSE_H
