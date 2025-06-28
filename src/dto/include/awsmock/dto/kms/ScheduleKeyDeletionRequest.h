//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_SCHEDULE_KEX_DELETION_REQUEST_H
#define AWSMOCK_DTO_KMS_SCHEDULE_KEX_DELETION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

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
    struct ScheduleKeyDeletionRequest final : Common::BaseCounter<ScheduleKeyDeletionRequest> {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Pending window in days
         */
        long pendingWindowInDays = 30;

      private:

        friend ScheduleKeyDeletionRequest tag_invoke(boost::json::value_to_tag<ScheduleKeyDeletionRequest>, boost::json::value const &v) {
            ScheduleKeyDeletionRequest r;
            r.keyId = Core::Json::GetStringValue(v, "KeyId");
            r.pendingWindowInDays = Core::Json::GetLongValue(v, "PendingWindowInDays");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ScheduleKeyDeletionRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KeyId", obj.keyId},
                    {"PendingWindowInDays", obj.pendingWindowInDays},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_SCHEDULE_KEX_DELETION_REQUEST_H
