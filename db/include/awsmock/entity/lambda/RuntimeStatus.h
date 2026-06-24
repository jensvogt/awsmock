//
// Created by vogje01 on 6/20/26.
//

#pragma once

// C++ includes
#include <map>
#include <string>

namespace Awsmock::Database::Entity::Lambda {
    /**
     * @brief Lambda entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum RuntimeStatus {
        starting,
        idle,
        running,
        failed,
        stopped,
        unknown,
    };

    static std::map<RuntimeStatus, std::string> LambdaStateNames{
            {starting, "starting"},
            {idle, "idle"},
            {running, "running"},
            {failed, "failed"},
            {stopped, "stopped"},
            {unknown, "unknown"},
    };

    [[maybe_unused]] static std::string RuntimeStatusToString(const RuntimeStatus &lambdaState) {
        return LambdaStateNames[lambdaState];
    }

    [[maybe_unused]] static RuntimeStatus RuntimeStatusFromString(const std::string &lambdaState) {
        for (auto &[fst, snd]: LambdaStateNames) {
            if (snd == lambdaState) {
                return fst;
            }
        }
        return unknown;
    }
}// namespace Awsmock::Database::Entity::Lambda