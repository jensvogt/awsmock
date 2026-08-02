//
// Created by vogje01 on 6/20/26.
//

#pragma once

// C++ includes
#include <map>
#include <string>

namespace Awsmock::Database::Entity::Lambda {
    /**
     * @brief Lambda invocation result status
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class LambdaResultStatus {
        success,
        failed,
        unknown,
    };

    static std::map<LambdaResultStatus, std::string> LambdaResultStatusNames{
            {LambdaResultStatus::success, "success"},
            {LambdaResultStatus::failed, "failed"},
            {LambdaResultStatus::unknown, "unknown"},
    };

    [[maybe_unused]] static std::string LambdaResultStatusToString(const LambdaResultStatus &lambdaResultStatus) {
        return LambdaResultStatusNames[lambdaResultStatus];
    }

    [[maybe_unused]] static LambdaResultStatus LambdaResultStatusFromString(const std::string &lambdaResultStatus) {
        for (auto &[fst, snd]: LambdaResultStatusNames) {
            if (snd == lambdaResultStatus) {
                return fst;
            }
        }
        return LambdaResultStatus::unknown;
    }
}// namespace Awsmock::Database::Entity::Lambda
