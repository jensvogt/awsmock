//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_SCHEDULE_KEY_DELETION_RESPONSE_H
#define AWSMOCK_DTO_KMS_SCHEDULE_KEY_DELETION_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

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
    struct ScheduledKeyDeletionResponse final : Common::BaseCounter<ScheduledKeyDeletionResponse> {

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
        long pendingWindowInDays;

      private:

        friend ScheduledKeyDeletionResponse tag_invoke(boost::json::value_to_tag<ScheduledKeyDeletionResponse>, boost::json::value const &v) {
            ScheduledKeyDeletionResponse r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.keyState = Core::Json::GetStringValue(v, "KeyState");
            r.deletionDate = Core::Json::GetLongValue(v, "DeletionDate");
            r.pendingWindowInDays = Core::Json::GetLongValue(v, "PendingWindowInDays");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ScheduledKeyDeletionResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"KeyState", obj.keyState},
                    {"DeletionDate", obj.deletionDate},
                    {"PendingWindowInDays", obj.pendingWindowInDays},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_SCHEDULE_KEY_DELETION_RESPONSE_H
