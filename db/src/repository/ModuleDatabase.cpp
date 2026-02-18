//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/ModuleDatabase.h>

namespace AwsMock::Database {

    std::map<std::string, Entity::Module::Module> ModuleDatabase::_existingModules = {
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
        {"application", {.name = "application", .state = Entity::Module::ModuleState::STOPPED, .status = Entity::Module::ModuleStatus::INACTIVE}}
    };

    void ModuleDatabase::Initialize() {

        for (auto &[fst, snd]: _existingModules) {
            if (HasDatabase()) {

                if (!ModuleExists(fst)) {
                    CreateModule(snd);
                }

            } else {

                if (!_memoryDb.ModuleExists(fst)) {
                    _memoryDb.CreateModule(snd);
                }
            }
        }
    }

    std::map<std::string, Entity::Module::Module> ModuleDatabase::GetExisting() {
        return _existingModules;
    }

    bool ModuleDatabase::IsActive(const std::string &name) {

        if (HasDatabase()) {

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

        } else {

            return _memoryDb.IsActive(name);
        }
        return false;
    }

    bool ModuleDatabase::ModuleExists(const std::string &name) {

        if (HasDatabase()) {

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

        } else {

            return _memoryDb.ModuleExists(name);
        }
        return false;
    }

    Entity::Module::Module ModuleDatabase::GetModuleById(const bsoncxx::oid &oid) const {

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

    Entity::Module::Module ModuleDatabase::GetModuleById(const std::string &oid) {

        if (HasDatabase()) {

            return GetModuleById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetModuleById(oid);
    }

    Entity::Module::Module ModuleDatabase::GetModuleByName(const std::string &name) {

        if (HasDatabase()) {

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
        } else {

            return _memoryDb.GetModuleByName(name);
        }
        return {};
    }

    std::vector<std::string> ModuleDatabase::GetAllModuleNames() {

        if (HasDatabase()) {

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
        return _memoryDb.GetAllModuleNames();
    }

    Entity::Module::Module ModuleDatabase::CreateModule(Entity::Module::Module &module) {

        if (HasDatabase()) {

            try {

                auto client = ConnectionPool::instance().GetConnection();
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
        return _memoryDb.CreateModule(module);
    }

    Entity::Module::Module ModuleDatabase::UpdateModule(Entity::Module::Module &module) {

        if (HasDatabase()) {

            try {
                mongocxx::options::find_one_and_update opts{};
                opts.return_document(mongocxx::options::return_document::k_after);

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
                const auto mResult = _moduleCollection.find_one_and_update(make_document(kvp("name", module.name)), module.ToDocument(), opts);
                log_trace << "Module updated: " << module.ToString();

                if (mResult) {
                    module.FromDocument(mResult->view());
                    return module;
                }
                return {};

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Update module failed, error: " << e.what();
                throw Core::DatabaseException("Update module failed, error: " + std::string(e.what()));
            }

        } else {

            return _memoryDb.UpdateModule(module);
        }
    }

    Entity::Module::Module ModuleDatabase::SetState(const std::string &name, const Entity::Module::ModuleState &state) {

        if (HasDatabase()) {
            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);
            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto mResult = _moduleCollection.find_one_and_update(make_document(kvp("name", name)), make_document(kvp("$set", make_document(kvp("state", ModuleStateToString(state))))), opts);
                log_trace << "Module state updated, name: " << name << " state: " << ModuleStateToString(state);
                session.commit_transaction();
                if (mResult) {
                    Entity::Module::Module module;
                    module.FromDocument(mResult->view());
                    return module;
                }
                return {};

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Set module state failed, error: " << e.what();
                session.abort_transaction();
            }
        }
        return _memoryDb.SetState(name, state);
    }

    Entity::Module::ModuleState ModuleDatabase::GetState(const std::string &name) {

        if (HasDatabase()) {

            const Entity::Module::Module module = GetModuleByName(name);
            log_trace << "Module state, name: " << name;
            return module.state;
        }
        return _memoryDb.GetModuleByName(name).state;
    }

    void ModuleDatabase::SetStatus(const std::string &name, const Entity::Module::ModuleStatus &status) {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
                auto mResult = _moduleCollection.update_one(make_document(kvp("name", name)), make_document(kvp("$set", make_document(kvp("status", ModuleStatusToString(status))))));
                log_trace << "Module status updated, name: " << name << " state: " << ModuleStatusToString(status);

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Set module status failed, error: " << e.what();
            }

        } else {

            _memoryDb.SetStatus(name, status);
        }
    }

    void ModuleDatabase::SetPort(const std::string &name, int port) {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
                auto mResult = _moduleCollection.update_one(make_document(kvp("name", name)),
                                                            make_document(kvp("$set", make_document(kvp("port", port)))));
                log_trace << "Module port updated, name: " << name << " port: " << port;

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Set module port failed, error: " << e.what();
            }

        } else {

            _memoryDb.SetPort(name, port);
        }
    }

    Entity::Module::Module ModuleDatabase::CreateOrUpdateModule(Entity::Module::Module &modules) {

        if (ModuleExists(modules.name)) {
            return UpdateModule((modules));
        }
        return CreateModule(modules);
    }

    int ModuleDatabase::ModuleCount() const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];
                const int64_t count = _moduleCollection.count_documents(make_document());
                log_trace << "Service state: " << std::boolalpha << count;
                return static_cast<int>(count);

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Service exists failed, error: " << e.what();
            }

        } else {

            return _memoryDb.ModuleCount();
        }
        return -1;
    }

    Entity::Module::ModuleList ModuleDatabase::ListModules() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

            Entity::Module::ModuleList modulesList;
            for (auto serviceCursor = _moduleCollection.find({}); auto service: serviceCursor) {
                Entity::Module::Module result;
                result.FromDocument(service);
                modulesList.push_back(result);
            }
            log_trace << "Got module list, size:" << modulesList.size();
            return modulesList;
        }
        return _memoryDb.ListModules();
    }

    void ModuleDatabase::DeleteModule(const Entity::Module::Module &module) {

        if (HasDatabase()) {

            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

                const auto result = _moduleCollection.delete_many(make_document(kvp("name", module.name)));
                log_debug << "Module deleted, count: " << result->deleted_count();

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Delete module failed, error: " << e.what();
            }

        } else {

            _memoryDb.DeleteModule(module);
        }
    }

    long ModuleDatabase::DeleteAllModules() {

        if (HasDatabase()) {

            try {
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _moduleCollection = (*client)[_databaseName][_moduleCollectionName];

                const auto result = _moduleCollection.delete_many(make_document());
                log_debug << "All module deleted, count: " << result->deleted_count();
                return result->deleted_count();

            } catch (mongocxx::exception::system_error &e) {
                log_error << "Delete all module failed, error: " << e.what();
            }
        }
        return _memoryDb.DeleteAllModules();
    }

} // namespace AwsMock::Database
