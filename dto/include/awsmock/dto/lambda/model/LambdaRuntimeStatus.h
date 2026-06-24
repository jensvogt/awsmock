//
// Created by vogje01 on 6/18/26.
//

#pragma once

// C++ includes
#include <ostream>
#include <stdexcept>
#include <string>

namespace Awsmock::Dto::Lambda {

    enum class LambdaRuntimeStatus {
        starting,
        idle,
        running,
        failed,
        stopped,
        unknown,
    };

    inline std::string runtimeStatusToString(const LambdaRuntimeStatus s) {
        switch (s) {
            case LambdaRuntimeStatus::starting:
                return "starting";
            case LambdaRuntimeStatus::idle:
                return "idle";
            case LambdaRuntimeStatus::running:
                return "running";
            case LambdaRuntimeStatus::failed:
                return "failed";
            case LambdaRuntimeStatus::stopped:
                return "stopped";
            case LambdaRuntimeStatus::unknown:
                return "unknown";
        }
        throw std::invalid_argument("unknown LambdaRuntimeStatus");
    }

    inline LambdaRuntimeStatus runtimeStatusFromString(const std::string &s) {
        if (s == "starting") return LambdaRuntimeStatus::starting;
        if (s == "idle") return LambdaRuntimeStatus::idle;
        if (s == "running") return LambdaRuntimeStatus::running;
        if (s == "failed") return LambdaRuntimeStatus::failed;
        if (s == "stopped") return LambdaRuntimeStatus::stopped;
        if (s == "unknown") return LambdaRuntimeStatus::unknown;
        throw std::invalid_argument("unknown LambdaRuntimeStatus: " + s);
    }

    inline std::ostream &operator<<(std::ostream &os, const LambdaRuntimeStatus s) {
        os << runtimeStatusToString(s);
        return os;
    }
}// namespace Awsmock::Dto::Lambda