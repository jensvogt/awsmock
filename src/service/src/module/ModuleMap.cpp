//
// Created by vogje01 on 10/6/24.
//

#include <awsmock/service/module/ModuleMap.h>

namespace AwsMock::Service {

    ModuleMap::ModuleMap() = default;

    void ModuleMap::AddModule(const std::string &name, const std::shared_ptr<AbstractServer> &server) {
        moduleMap[name] = server;
    }

    std::shared_ptr<AbstractServer> ModuleMap::GetModule(const std::string &name) {
        return moduleMap[name];
    }

    bool ModuleMap::HasModule(const std::string &name) const {
        return moduleMap.contains(name);
    }

    std::map<std::string, std::shared_ptr<AbstractServer>> ModuleMap::GetModuleMap() {
        return moduleMap;
    }

}// namespace AwsMock::Service