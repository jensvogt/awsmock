//
// Created by vogje01 on 10/22/23.
//

#pragma once

// C++ includes
#include <map>
#include <string>

namespace Awsmock::Database::Entity {

    /**
     * @enum ModuleState
     * @brief Represents the various states a module can be in during its lifecycle.
     *
     * This enumeration defines the following possible states:
     * - STOPPED: The module is not active or running.
     * - STARTING: The module is in the process of starting up.
     * - RUNNING: The module is currently active and operational.
     * - STOPPING: The module is in the process of shutting down.
     * - CRASHED: The module has encountered a failure and stopped unexpectedly.
     * - PENDING_RESTART: The module is flagged for a restart but has not yet restarted.
     * - RESTARTING: The module is in the process of restarting after a failure or manual intervention.
     * - UNKNOWN: The state of the module is not recognized or cannot be determined.
     */
    enum class ProcessState {
        STOPPED,
        STARTING,
        RUNNING,
        STOPPING,
        CRASHED,
        PENDING_RESTART,
        RESTARTING,
        UNKNOWN
    };

    /**
     * @brief Maps ModuleState enumeration values to their corresponding string representations.
     *
     * This static variable provides a mapping between the `ModuleState` enum and its
     * string equivalents. It is used for converting `ModuleState` values to human-readable
     * strings or for managing state-related operations where string representations are required.
     *
     * Supported mappings include:
     * - STOPPED: "STOPPED"
     * - STARTING: "STARTING"
     * - RUNNING: "RUNNING"
     * - STOPPING: "STOPPING"
     * - CRASHED: "CRASHED"
     * - PENDING_RESTART: "PENDING_RESTART"
     * - RESTARTING: "RESTARTING"
     * - UNKNOWN: "UNKNOWN"
     */
    static std::map<ProcessState, std::string> ProcessStateNames{
            {ProcessState::STOPPED, "STOPPED"},
            {ProcessState::STARTING, "STARTING"},
            {ProcessState::RUNNING, "RUNNING"},
            {ProcessState::STOPPING, "STOPPING"},
            {ProcessState::CRASHED, "CRASHED"},
            {ProcessState::PENDING_RESTART, "PENDING_RESTART"},
            {ProcessState::RESTARTING, "RESTARTING"},
            {ProcessState::UNKNOWN, "UNKNOWN"},
    };

    /**
     * Converts a ModuleState enum value to its corresponding string representation.
     *
     * @param processState The process states an enum value to be converted.
     * @return A string representation of the given ModuleState value.
     */
    [[maybe_unused]]
    static std::string ProcessStateToString(const ProcessState &processState) {
        return ProcessStateNames[processState];
    }

    /**
     * Converts a string representation of a module state to its corresponding ModuleState enum value.
     *
     * The method searches for the given string in the predefined map of module states. If the string matches
     * one of the defined state names, the corresponding enum value is returned. If no match is found, the
     * method returns ModuleState::UNKNOWN.
     *
     * @param processState The string representation of the module state to convert.
     * @return The corresponding ModuleState enum value, or ModuleState::UNKNOWN if the string does not match any known state.
     */
    [[maybe_unused]] static ProcessState ProcessStateFromString(const std::string &processState) {
        const auto it = std::ranges::find_if(ProcessStateNames, [&processState](const auto &pair) { return pair.second == processState; });
        return it != ProcessStateNames.end() ? it->first : ProcessState::UNKNOWN;
    }

}// namespace Awsmock::Database::Entity
