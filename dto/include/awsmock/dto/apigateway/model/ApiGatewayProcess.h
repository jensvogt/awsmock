//
// Created by jensv on 23/05/2026.
//

#pragma once

// C++ includes
#include <chrono>
#include <string>
#ifdef _WIN32
#include <windows.h>
using pid_t = DWORD;
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <vector>

// Awsmock includes
#include <awsmock/dto/apigateway/model/ProcessConfig.h>
#include <awsmock/entity/apigateway/ProcessState.h>

namespace Awsmock::Dto {

    struct ModuleProcess {

        /**
         * @brief Module config
         */
        ProcessConfig config;

        /**
         * @brief Process ID
         */
        pid_t pid = -1;

        /**
         * @brief Module state
         */
        Database::Entity::ProcessState state = Database::Entity::ProcessState::STOPPED;

        /**
         * @brief Restart count
         */
        int restartCount = 0;

        /**
         * @brief Current restart delay
         */
        int currentDelay = 1000;

        /**
         * @brief Process start time
         */
        std::chrono::steady_clock::time_point startTime;

        /**
         * @brief Time of last crash
         */
        std::chrono::steady_clock::time_point lastCrashTime;

        /**
         * @brief stdout capture
         */
        int stdoutFd = -1;

        /**
         * @brief stderr capture
         */
        int stderrFd = -1;
    };
}// namespace Awsmock::Dto