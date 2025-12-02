//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_RESTORE_STATUS_H
#define AWSMOCK_RESTORE_STATUS_H

// C++ includes
#include <chrono>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief S3 object restore status DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RestoreStatus final : Common::BaseCounter<RestoreStatus> {

        /**
         * Is restore in progress
         */
        bool isRestoreInProgress = false;

        /**
         * Expiration datetime
         */
        system_clock::time_point restoreExpiryDate;

      private:

        friend RestoreStatus tag_invoke(boost::json::value_to_tag<RestoreStatus>, boost::json::value const &v) {
            RestoreStatus r;
            r.isRestoreInProgress = Core::Json::GetBoolValue(v, "isRestoreInProgress");
            r.restoreExpiryDate = Core::Json::GetDatetimeValue(v, "Owner");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RestoreStatus const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"isRestoreInProgress", obj.isRestoreInProgress},
                    {"restoreExpiryDate", Core::DateTimeUtils::ToISO8601(obj.restoreExpiryDate)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_RESTORE_STATUS_H
