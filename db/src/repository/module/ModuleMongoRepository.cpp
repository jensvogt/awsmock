//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/module/ModuleMongoRepository.h>

namespace Awsmock::Database {

    std::map<std::string, Entity::Module::Module> ModuleMongoRepository::_existingModules = {
            {"s3", {.name = "s3", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"sqs", {.name = "sqs", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"sns", {.name = "sns", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"lambda", {.name = "lambda", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"transfer", {.name = "transfer", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"cognito", {.name = "cognito", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"dynamodb", {.name = "dynamodb", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"secretsmanager", {.name = "secretsmanager", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"kms", {.name = "kms", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"ssm", {.name = "ssm", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"application", {.name = "application", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}},
            {"api-gateway", {.name = "api-gateway", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}}};

    void ModuleMongoRepository::initialize() const {

        for (auto &[fst, snd]: _existingModules) {

            if (!moduleExists(fst)) {
                Entity::Module::Module module = createModule(snd);
                log_debug << "Module created, name: " << module.name;
            }
        }
    }

    std::map<std::string, Entity::Module::Module> ModuleMongoRepository::getExisting() {
        return _existingModules;
    }

    bool ModuleMongoRepository::isActive(const std::string &name) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            if (auto result = _moduleCollection.find_one(make_document(kvp("name", name)))) {
                Entity::Module::Module module;
                module.FromDocument(result->view());
                log_trace << "Module state: " << ModuleStateToString(module.state);
                return module.status == Entity::Module::ModuleStatus::ACTIVE;
            }

        } catch (mongocxx::exception::system_error &e) {
            log_error << "IsActive failed, module: " << name << " error: " << e.what();
        }
        return false;
    }

    bool ModuleMongoRepository::moduleExists(const std::string &name) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

            document query{};
            if (!name.empty()) {
                query.append(kvp("name", name));
            }

            const auto result = _moduleCollection.find_one(query.extract());
            log_trace << "Module exists, name: " << name << ", exists: " << std::boolalpha << result.has_value();
            return result.has_value();

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Module exists failed, name: " << ", error: " << e.what();
        }
        return false;
    }

    Entity::Module::Module ModuleMongoRepository::getModuleById(const bsoncxx::oid &oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

            if (auto mResult = _moduleCollection.find_one(make_document(kvp("_id", oid)))) {
                Entity::Module::Module module;
                module.FromDocument(mResult->view());
                return module;
            }

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Get module by ID failed, error: " << e.what();
            throw Core::DatabaseException("Module not found, oid: " + oid.to_string());
        }
        return {};
    }

    Entity::Module::Module ModuleMongoRepository::getModuleById(const std::string &oid) const {
        return getModuleById(bsoncxx::oid(oid));
    }

    Entity::Module::Module ModuleMongoRepository::getModuleByName(const std::string &name) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            if (auto mResult = _moduleCollection.find_one(make_document(kvp("name", name)))) {
                Entity::Module::Module modules;
                modules.FromDocument(mResult->view());
                return modules;
            }

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Get module by name failed, name: " << name << " error: " << e.what();
        }
        return {};
    }

    std::vector<std::string> ModuleMongoRepository::getAllModuleNames() const {

        try {

            std::vector<std::string> moduleNameList;
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            for (auto serviceCursor = _moduleCollection.find({}); auto service: serviceCursor) {
                Entity::Module::Module result;
                result.FromDocument(service);
                moduleNameList.push_back(result.name);
            }
            return moduleNameList;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Get module names, error: " << e.what();
            return {};
        }
    }

    Entity::Module::Module ModuleMongoRepository::createModule(Entity::Module::Module &module) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            auto mResult = _moduleCollection.insert_one(module.ToDocument());
            log_trace << "Module created, oid: " << mResult->inserted_id().get_oid().value.to_string();

            if (mResult) {
                module.oid = mResult->inserted_id().get_oid().value.to_string();
                return module;
            }
            return {};

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Create module failed, error: " << e.what();
            throw Core::DatabaseException("Create module failed, error: " + std::string(e.what()));
        }
    }

    Entity::Module::Module ModuleMongoRepository::updateModule(Entity::Module::Module &module) const {

        try {
            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            const auto mResult = _moduleCollection.find_one_and_update(make_document(kvp("name", module.name)), module.ToDocument(), opts);
            log_trace << "Module updated: " << module.name;

            if (mResult) {
                module.FromDocument(mResult->view());
                return module;
            }
            return {};

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Update module failed, error: " << e.what();
            throw Core::DatabaseException("Update module failed, error: " + std::string(e.what()));
        }
    }

    void ModuleMongoRepository::setState(const std::string &name, const Entity::Module::ModuleState &state) const {

        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);
        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

        try {

            const auto mResult = _moduleCollection.find_one_and_update(make_document(kvp("name", name)), make_document(kvp("$set", make_document(kvp("state", ModuleStateToString(state))))), opts);
            log_trace << "Module state updated, name: " << name << " state: " << ModuleStateToString(state);

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Set module state failed, error: " << e.what();
        }
    }

    Entity::Module::ModuleState ModuleMongoRepository::getState(const std::string &name) const {
        const Entity::Module::Module module = getModuleByName(name);
        log_trace << "Module state, name: " << name;
        return module.state;
    }

    void ModuleMongoRepository::setStatus(const std::string &name, const Entity::Module::ModuleStatus &status) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            auto mResult = _moduleCollection.update_one(make_document(kvp("name", name)), make_document(kvp("$set", make_document(kvp("status", ModuleStatusToString(status))))));
            log_trace << "Module status updated, name: " << name << " state: " << ModuleStatusToString(status);

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Set module status failed, error: " << e.what();
        }
    }

    void ModuleMongoRepository::setModulePort(const std::string &name, int port) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            auto mResult = _moduleCollection.update_one(make_document(kvp("name", name)),
                                                        make_document(kvp("$set", make_document(kvp("port", port)))));
            log_trace << "Module port updated, name: " << name << " port: " << port;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Set module port failed, error: " << e.what();
        }
    }

    Entity::Module::Module ModuleMongoRepository::createOrUpdateModule(Entity::Module::Module &modules) const {

        if (moduleExists(modules.name)) {
            return updateModule((modules));
        }
        return createModule(modules);
    }

    int ModuleMongoRepository::moduleCount() const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            const int64_t count = _moduleCollection.count_documents(make_document());
            log_trace << "Service state: " << std::boolalpha << count;
            return static_cast<int>(count);

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Service exists failed, error: " << e.what();
        }
        return -1;
    }

    std::vector<Entity::Module::Module> ModuleMongoRepository::listModules() const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

        std::vector<Entity::Module::Module> modulesList;
        for (auto serviceCursor = _moduleCollection.find({}); auto service: serviceCursor) {
            Entity::Module::Module result;
            result.FromDocument(service);
            modulesList.push_back(result);
        }
        log_trace << "Got module list, size:" << modulesList.size();
        return modulesList;
    }

    long ModuleMongoRepository::setModuleLoglevel(const std::string &name, const std::string &level) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

            const auto result = _moduleCollection.update_one(make_document(kvp("name", name)), make_document(kvp("$set", make_document(kvp("logLevel", level)))));
            log_debug << "Module logLevel updated, name: " << name << " level: " << level;
            return result ? result->modified_count() : 0;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Set module loglevel failed, error: " << e.what();
        }
        return -1;
    }

    long ModuleMongoRepository::setAllModulesLoglevel(const std::string &level) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

            const auto result = _moduleCollection.update_many(make_document(), make_document(kvp("$set", make_document(kvp("logLevel", level)))));
            log_debug << "All modules updated, logLevel: " << level;
            return result ? result->modified_count() : 0;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Set all modules loglevel failed, error: " << e.what();
        }
        return -1;
    }

    void ModuleMongoRepository::setModuleLogChannelAndLevel(const std::string &name, const std::string &channel, const std::string &level) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            _moduleCollection.update_one(
                    make_document(kvp("name", name)),
                    make_document(kvp("$set", make_document(kvp("logChannel", channel), kvp("logLevel", level)))));
            log_trace << "Module log channel/level updated, name: " << name << " channel: " << channel << " level: " << level;

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Set module log channel/level failed, error: " << e.what();
        }
    }

    void ModuleMongoRepository::deleteModule(const Entity::Module::Module &module) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

            const auto result = _moduleCollection.delete_many(make_document(kvp("name", module.name)));
            log_debug << "Module deleted, count: " << result->deleted_count();

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Delete module failed, error: " << e.what();
        }
    }

    long ModuleMongoRepository::deleteAllModules() const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

            const auto result = _moduleCollection.delete_many(make_document());
            log_debug << "All module deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (mongocxx::exception::system_error &e) {
            log_error << "Delete all module failed, error: " << e.what();
        }
        return -1;
    }

}// namespace Awsmock::Database
