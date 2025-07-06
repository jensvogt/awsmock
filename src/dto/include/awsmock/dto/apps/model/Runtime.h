//
// Created by vogje01 on 6/5/24.
//

#ifndef AWSMOCK_DTO_APPS_RUNTIME_H
#define AWSMOCK_DTO_APPS_RUNTIME_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Application runtimes
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class AppsRuntimeType {
        JAVA_11,
        JAVA_17,
        JAVA_21,
        UNKNOWN
    };

    static std::map<AppsRuntimeType, std::string> AppsRuntimeTypeNames{
            {AppsRuntimeType::JAVA_11, "JAVA_11"},
            {AppsRuntimeType::JAVA_17, "JAVA_17"},
            {AppsRuntimeType::JAVA_21, "JAVA_21"},
            {AppsRuntimeType::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]] static std::string AppsRuntimeTypeToString(const AppsRuntimeType &runtimeType) {
        return AppsRuntimeTypeNames[runtimeType];
    }

    [[maybe_unused]] static AppsRuntimeType AppsRuntimeTypeFromString(const std::string &runtimeType) {
        for (auto &[fst, snd]: AppsRuntimeTypeNames) {
            if (snd == runtimeType) {
                return fst;
            }
        }
        return AppsRuntimeType::UNKNOWN;
    }

}// namespace AwsMock::Dto::Apps

#endif//AWSMOCK_DTO_APPS_RUNTIME_H
