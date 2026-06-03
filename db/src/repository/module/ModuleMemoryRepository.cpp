//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/module/ModuleMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex ModuleMemoryRepository::_moduleMutex;

    void ModuleMemoryRepository::initialize() const {}

    bool ModuleMemoryRepository::moduleExists(const std::string &name) const {

        return std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                   return module.second.name == name;
               }) != _modules.end();
    }

    bool ModuleMemoryRepository::isActive(const std::string &name) const {

        return std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                   return module.second.name == name && module.second.status == Entity::Module::ModuleStatus::ACTIVE;
               }) != _modules.end();
    }

    Entity::Module::Module ModuleMemoryRepository::getModuleById(const std::string &oid) const {

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

    Entity::Module::Module ModuleMemoryRepository::getModuleById(const bsoncxx::oid &oid) const {
        return getModuleById(oid.to_string());
    }

    Entity::Module::Module ModuleMemoryRepository::getModuleByName(const std::string &name) const {

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

    std::vector<std::string> ModuleMemoryRepository::getAllModuleNames() const {

        std::vector<std::string> moduleNameList;
        for (const auto &val: _modules | std::views::values) {
            moduleNameList.emplace_back(val.name);
        }

        log_trace << "Got module name list, size: " << moduleNameList.size();
        return moduleNameList;
    }

    Entity::Module::ModuleState ModuleMemoryRepository::getState(const std::string &name) const {

        const Entity::Module::Module module = getModuleByName(name);
        return module.state;
    }

    Entity::Module::Module ModuleMemoryRepository::createModule(Entity::Module::Module &module) const {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _modules[oid] = module;
        log_trace << "Module created, oid: " << oid;
        return _modules[oid];
    }

    Entity::Module::Module ModuleMemoryRepository::createOrUpdateModule(Entity::Module::Module &module) const {
        if (moduleExists(module.name)) {
            return updateModule(module);
        }
        return createModule(module);
    }

    std::vector<Entity::Module::Module> ModuleMemoryRepository::listModules() const {

        std::vector<Entity::Module::Module> moduleList;
        for (const auto &val: _modules | std::views::values) {
            moduleList.emplace_back(val);
        }

        log_trace << "Got module list, size: " << moduleList.size();
        return moduleList;
    }

    Entity::Module::Module ModuleMemoryRepository::updateModule(Entity::Module::Module &module) const {
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

    void ModuleMemoryRepository::setState(const std::string &name, const Entity::Module::ModuleState &state) const {
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
    }

    void ModuleMemoryRepository::setStatus(const std::string &name, const Entity::Module::ModuleStatus &status) const {
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

    void ModuleMemoryRepository::setModulePort(const std::string &name, int port) const {
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

    int ModuleMemoryRepository::moduleCount() const {
        return static_cast<int>(_modules.size());
    }

    void ModuleMemoryRepository::deleteModule(const Entity::Module::Module &module) const {
        boost::mutex::scoped_lock lock(_moduleMutex);

        std::string name = module.name;
        const auto count = std::erase_if(_modules, [name](const auto &item) {
            auto const &[key, value] = item;
            return value.name == name;
        });
        log_debug << "Module deleted, count: " << count;
    }

    long ModuleMemoryRepository::setModuleLoglevel(const std::string &name, const std::string &level) const {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const auto it =
                std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.second.name == name;
                });

        if (it == _modules.end()) {
            log_error << "Set logLevel failed, module: " << name;
            throw Core::DatabaseException("Set logLevel failed, module: " + name);
        }

        it->second.logLevel = level;
        _modules[it->first] = it->second;
        return 1;
    }

    long ModuleMemoryRepository::setAllModulesLoglevel(const std::string &level) const {
        boost::mutex::scoped_lock lock(_moduleMutex);

        for (auto &module: _modules | std::views::values) {
            module.logLevel = level;
        }
        log_debug << "All module logLevels updated, count: " << _modules.size();
        return static_cast<long>(_modules.size());
    }

    long ModuleMemoryRepository::deleteAllModules() const {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const long count = static_cast<long>(_modules.size());
        log_debug << "All modules deleted, count: " << _modules.size();
        _modules.clear();
        return count;
    }

    void ModuleMemoryRepository::setModuleLogChannelAndLevel(const std::string &name, const std::string &channel, const std::string &level) const {
        boost::mutex::scoped_lock lock(_moduleMutex);

        const auto it =
                std::ranges::find_if(_modules, [name](const std::pair<std::string, Entity::Module::Module> &module) {
                    return module.second.name == name;
                });

        if (it == _modules.end()) {
            log_error << "Set logChannel/logLevel failed, module: " << name;
            throw Core::DatabaseException("Set logChannel/logLevel failed, module: " + name);
        }

        it->second.logChannel = channel;
        it->second.logLevel = level;
        _modules[it->first] = it->second;
    }
}// namespace Awsmock::Database
