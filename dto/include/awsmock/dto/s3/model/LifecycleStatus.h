//
// Created by vogje01 on 1/9/26.
//

#ifndef AWSMOCK_DTO_S3_LIFECYCLE_STATUS_H
#define AWSMOCK_DTO_S3_LIFECYCLE_STATUS_H

// AwsMock includes
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Versioning status
     */
    enum LifecycleStatus {
        LIFECYCLE_ENABLED,
        LIFECYCLE_DISABLED
    };

    static std::map<LifecycleStatus, std::string> LifeCycleStatusNames{
            {LIFECYCLE_ENABLED, "Enabled"},
            {LIFECYCLE_DISABLED, "Disabled"},
    };

    [[maybe_unused]] static std::string LifeCycleStatusToString(const LifecycleStatus lifeCycleStatus) {
        return LifeCycleStatusNames[lifeCycleStatus];
    }

    [[maybe_unused]] static LifecycleStatus LifeCycleStatusFromString(const std::string &lifeCycleStatus) {
        for (auto &[fst, snd]: LifeCycleStatusNames) {
            if (Core::StringUtils::EqualsIgnoreCase(snd, lifeCycleStatus)) {
                return fst;
            }
        }
        return LIFECYCLE_DISABLED;
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIFECYCLE_STATUS_H
