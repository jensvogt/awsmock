//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/ModuleMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex ModuleMemoryDb::_moduleMutex;

    bool ModuleMemoryDb::ModuleExists(const std::string &name) {

        return std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                   return module.second.name == name;
               }) != _modules.end();
    }

    bool ModuleMemoryDb::IsActive(const std::string &name) {

        return std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                   return module.second.name == name && module.second.status == Entity::Module::ModuleStatus::ACTIVE;
               }) != _modules.end();
    }

    Entity::Module::Module ModuleMemoryDb::GetModuleById(const std::string &oid) {

        const auto it =
                std::ranges::find_if(_modules, [oid](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.first == oid;
                });

        if (it == _modules.end()) {
            log_error << "Get module by ID failed, oid: " << oid;
            throw Core::DatabaseException("Get module by ID failed, oid: " + oid);
        }
        it->second.oid = oid;
        return it->second;
    }

    Entity::Module::Module ModuleMemoryDb::GetModuleByName(const std::string &name) {

        const auto it =
                std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.second.name == name;
                });

        if (it == _modules.end()) {
            log_error << "Get module by name failed, oid: " << name;
            throw Core::DatabaseException("Get module by name failed, oid: " + name);
        }
        it->second.oid = it->first;
        return it->second;
    }

    std::vector<std::string> ModuleMemoryDb::GetAllModuleNames() {

        std::vector<std::string> moduleNameList;
        for (const auto &val: _modules | std::views::values) {
            moduleNameList.emplace_back(val.name);
        }

        log_trace << "Got module name list, size: " << moduleNameList.size();
        return moduleNameList;
    }

    Entity::Module::Module ModuleMemoryDb::CreateModule(const Entity::Module::Module &module) {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _modules[oid] = module;
        log_trace << "Module created, oid: " << oid;
        return GetModuleById(oid);
    }

    Entity::Module::ModuleList ModuleMemoryDb::ListModules() const {

        Entity::Module::ModuleList moduleList;
        for (const auto &val: _modules | std::views::values) {
            moduleList.emplace_back(val);
        }

        log_trace << "Got module list, size: " << moduleList.size();
        return moduleList;
    }

    Entity::Module::Module ModuleMemoryDb::UpdateModule(const Entity::Module::Module &module) {
        boost::mutex::scoped_lock lock(_moduleMutex);

        std::string name = module.name;
        const auto it =
                std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.second.name == name;
                });

        if (it == _modules.end()) {
            log_error << "Update module failed, module: " << name;
            throw Core::DatabaseException("Update module failed, module: " + name);
        }

        _modules[it->first] = module;
        return _modules[it->first];
    }

    Entity::Module::Module ModuleMemoryDb::SetState(const std::string &name, const Entity::Module::ModuleState &state) {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const auto it =
                std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.second.name == name;
                });

        if (it == _modules.end()) {
            log_error << "Set state failed, module: " << name;
            throw Core::DatabaseException("Set state failed, module: " + name);
        }

        it->second.state = state;
        _modules[it->first] = it->second;
        return it->second;
    }

    void ModuleMemoryDb::SetStatus(const std::string &name, const Entity::Module::ModuleStatus &status) {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const auto it =
                std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.second.name == name;
                });

        if (it == _modules.end()) {
            log_error << "Set status failed, module: " << name;
            throw Core::DatabaseException("Set status failed, module: " + name);
        }

        it->second.status = status;
        _modules[it->first] = it->second;
    }

    void ModuleMemoryDb::SetPort(const std::string &name, int port) {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const auto it =
                std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.second.name == name;
                });

        if (it == _modules.end()) {
            log_error << "Set port failed, module: " << name;
            throw Core::DatabaseException("Set port failed, module: " + name);
        }

        it->second.port = port;
        _modules[it->first] = it->second;
    }

    int ModuleMemoryDb::ModuleCount() const {
        return static_cast<int>(_modules.size());
    }

    void ModuleMemoryDb::DeleteModule(const Entity::Module::Module &module) {
        boost::mutex::scoped_lock lock(_moduleMutex);

        std::string name = module.name;
        const auto count = std::erase_if(_modules, [name](const auto &item) {
            auto const &[key, value] = item;
            return value.name == name;
        });
        log_debug << "Module deleted, count: " << count;
    }

    long ModuleMemoryDb::DeleteAllModules() {
        boost::mutex::scoped_lock lock(_moduleMutex);

        long count = _modules.size();
        log_debug << "All modules deleted, count: " << _modules.size();
        _modules.clear();
        return count;
    }
}// namespace AwsMock::Database