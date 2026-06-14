//
// Created by vogje01 on 5/10/24.
//

#pragma once

// C++ includes
#include <chrono>

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief S3 object restore status DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RestoreStatus final : Common::BaseObject<RestoreStatus> {

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
                    {"isRestoreInProgress", obj.isRestoreInProgress},
                    {"restoreExpiryDate", Core::DateTimeUtils::ToISO8601(obj.restoreExpiryDate)},
            };
        }
    };

}// namespace Awsmock::Dto::S3
