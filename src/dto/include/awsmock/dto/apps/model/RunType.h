//
// Created by vogje01 on 6/5/24.
//

#ifndef AWSMOCK_DTO_APPS_RUNTYPE_H
#define AWSMOCK_DTO_APPS_RUNTYPE_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Application run type
     *
     * @par
     * Can be one of 'DOCKER', 'LOCAL'. In case the Docker run type is set the executable runs as docker container, with local the application runs
     * on the host machine.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class AppsRunType {
        LOCAL,
        DOCKER,
        UNKNOWN
    };

    static std::map<AppsRunType, std::string> AppsRunTypeNames{
            {AppsRunType::LOCAL, "LOCAL"},
            {AppsRunType::DOCKER, "DOCKER"},
            {AppsRunType::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]] static std::string AppsRunTypeToString(const AppsRunType &runType) {
        return AppsRunTypeNames[runType];
    }

    [[maybe_unused]] static AppsRunType AppsRunTypeFromString(const std::string &runType) {
        for (auto &[fst, snd]: AppsRunTypeNames) {
            if (snd == runType) {
                return fst;
            }
        }
        return AppsRunType::UNKNOWN;
    }

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_RUNTYPE_H
