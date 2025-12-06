//
// Created by vogje01 on 10/22/23.
//

#ifndef AWSMOCK_DB_ENTITY_MODULE_MODULE_STATUS_H
#define AWSMOCK_DB_ENTITY_MODULE_MODULE_STATUS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Database::Entity::Module {

    /**
     * @brief AwsMock module state
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class ModuleState {
        STOPPED,
        STARTING,
        RUNNING,
        SUSPENDED,
        UNKNOWN
    };

    static std::map<ModuleState, std::string> ModuleStateNames{
            {ModuleState::STOPPED, "STOPPED"},
            {ModuleState::STARTING, "STARTING"},
            {ModuleState::RUNNING, "RUNNING"},
            {ModuleState::SUSPENDED, "SUSPENDED"},
            {ModuleState::UNKNOWN, "UNKNOWN"},
    };

    [[maybe_unused]] static std::string ModuleStateToString(ModuleState serviceStatus) {
        return ModuleStateNames[serviceStatus];
    }

    [[maybe_unused]] static ModuleState ModuleStateFromString(const std::string &serviceStatus) {
        for (auto &[fst, snd]: ModuleStateNames) {
            if (snd == serviceStatus) {
                return fst;
            }
        }
        return ModuleState::UNKNOWN;
    }

    enum class ModuleStatus {
        INACTIVE,
        ACTIVE,
        UNKNOWN
    };

    static std::map<ModuleStatus, std::string> ModuleStatusNames{
            {ModuleStatus::INACTIVE, "INACTIVE"},
            {ModuleStatus::ACTIVE, "ACTIVE"},
            {ModuleStatus::UNKNOWN, "UNKNOWN"}};

    [[maybe_unused]] static std::string ModuleStatusToString(ModuleStatus modulesStatus) {
        return ModuleStatusNames[modulesStatus];
    }

    [[maybe_unused]] static ModuleStatus ModuleStatusFromString(const std::string &modulesStatus) {
        for (auto &[fst, snd]: ModuleStatusNames) {
            if (snd == modulesStatus) {
                return fst;
            }
        }
        return ModuleStatus::UNKNOWN;
    }

}// namespace AwsMock::Database::Entity::Module

#endif// AWSMOCK_DB_ENTITY_MODULE_MODULE_STATUS_H
