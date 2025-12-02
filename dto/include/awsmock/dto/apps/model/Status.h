//
// Created by vogje01 on 6/5/24.
//

#ifndef AWSMOCK_DTO_APPS_STATUS_H
#define AWSMOCK_DTO_APPS_STATUS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Application status
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class AppsStatusType {
        ACTIVE,
        PENDING,
        STOPPED,
        RUNNING,
        UNKNOWN
    };

    static std::map<AppsStatusType, std::string> AppsStatusTypeNames{
            {AppsStatusType::ACTIVE, "ACTIVE"},
            {AppsStatusType::PENDING, "PENDING"},
            {AppsStatusType::STOPPED, "STOPPED"},
            {AppsStatusType::RUNNING, "RUNNING"},
            {AppsStatusType::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]] static std::string AppsStatusTypeToString(const AppsStatusType &statusType) {
        return AppsStatusTypeNames[statusType];
    }

    [[maybe_unused]] static AppsStatusType AppsStatusTypeFromString(const std::string &statusType) {
        for (auto &[fst, snd]: AppsStatusTypeNames) {
            if (snd == statusType) {
                return fst;
            }
        }
        return AppsStatusType::UNKNOWN;
    }

}// namespace AwsMock::Dto::Apps

#endif//AWSMOCK_DTO_APPS_STATUS_H
