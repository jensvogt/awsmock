//
// Created by vogje01 on 10/6/24.
//

#include <awsmock/service/module/ModuleMap.h>

namespace AwsMock::Service {

    void ModuleMap::AddModule(const std::string &name, const std::shared_ptr<AbstractServer> &server) {
        _moduleMap[name] = server;
    }

    std::shared_ptr<AbstractServer> ModuleMap::GetModule(const std::string &name) {
        return _moduleMap[name];
    }

    bool ModuleMap::HasModule(const std::string &name) const {
        return _moduleMap.contains(name);
    }

    std::map<std::string, std::shared_ptr<AbstractServer>> ModuleMap::GetModuleMap() {
        return _moduleMap;
    }

    std::vector<std::string> ModuleMap::GetModuleNames() {
        std::vector<std::string> names;
        for (const auto &key: _moduleMap | std::views::keys) {
            names.push_back(key);
        }
        return names;
    }

    int ModuleMap::GetSize() const {
        return static_cast<int>(_moduleMap.size());
    }

}// namespace AwsMock::Service