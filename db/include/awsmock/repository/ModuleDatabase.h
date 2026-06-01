//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/memorydb/ModuleMemoryDb.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/ConnectionPool.h>

namespace AwsMock::Database {

    /**
     * @brief Module MongoDB database.
     *
     * Controls all the AwsMock modules.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ModuleDatabase : public DatabaseBase {

    public:
        /**
         * @brief Constructor
         */
        explicit ModuleDatabase() : _databaseName(GetDatabaseName()), _moduleCollectionName("module") {
        }

        /**
         * @brief Singleton instance
         */
        static ModuleDatabase &instance() {
            static ModuleDatabase moduleDatabase;
            return moduleDatabase;
        }

        /**
         * @brief Initialize the database
         */
        void Initialize();

        /**
         * @brief Returns a list of existing modules.
         *
         * @return map of existing modules
         */
        static std::map<std::string, Entity::Module::Module> GetExisting();

        /**
         * @brief Checks the active flag.
         *
         * @param name module name
         * @return true if active
         */
        bool IsActive(const std::string &name);

        /**
         * @brief Check the existence of a module
         *
         * @par
         * This method checks only the name of the module
         *
         * @param name name name
         * @return created name
         */
        bool ModuleExists(const std::string &name);

        /**
         * @brief Returns the module by OID
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::Module::Module GetModuleById(const bsoncxx::oid &oid) const;

        /**
         * @brief Returns the module by OID as string value
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        Entity::Module::Module GetModuleById(const std::string &oid);

        /**
         * @brief Returns the module by name
         *
         * @param name module name
         * @return module, if existing
         * @throws DatabaseException
         */
        Entity::Module::Module GetModuleByName(const std::string &name);

        /**
         * @brief Returns all module names
         *
         * @return list of module names
         * @throws DatabaseException
         */
        std::vector<std::string> GetAllModuleNames();

        /**
         * @brief Creates a new module
         *
         * @param module module entity
         * @return created module
         */
        Entity::Module::Module CreateModule(Entity::Module::Module &module);

        /**
         * @brief Updates an existing module
         *
         * @param module module entity
         * @return updated module
         */
        Entity::Module::Module UpdateModule(Entity::Module::Module &module);

        /**
         * @brief Sets the state of a module.
         *
         * <p>State can be one of STARTING, RUNNING, STOPPED, UNKNOWN.</p>
         *
         * @param name module name
         * @param state module state
         * @return updated module
         * @see AwsMock::Database::Entity::Module::ModuleState()
         */
        Entity::Module::Module SetState(const std::string &name, const Entity::Module::ModuleState &state);

        /**
         * @brief Sets the state of a module.
         *
         * <p>State can be one of STARTING, RUNNING, STOPPED, UNKNOWN.</p>
         *
         * @param name module name
         * @return module state
         * @see AwsMock::Database::Entity::Module::ModuleState()
         */
        Entity::Module::ModuleState GetState(const std::string &name);

        /**
         * @brief Sets the status of a module.
         *
         * <p>Status can be one of UNKNOWN, ACTIVE, INACTIVE.</p>
         *
         * @param name module name
         * @param status module status
         * @see AwsMock::Database::Entity::Module::ModuleStatus()
         */
        void SetStatus(const std::string &name, const Entity::Module::ModuleStatus &status);

        /**
         * @brief Sets the port of a module.
         *
         * @param name module name
         * @param port module port
         */
        void SetModulePort(const std::string &name, int port);

        /**
         * @brief Creates or updates a modules
         *
         * @param modules modules entity
         * @return updated modules
         */
        Entity::Module::Module CreateOrUpdateModule(Entity::Module::Module &modules);

        /**
         * @brief Counts the number of modules
         *
         * @return total number of modules
         */
        int ModuleCount() const;

        /**
         * @brief Returns a list of all modules
         *
         * @return list of all modules
         */
        [[nodiscard]]
        std::vector<Entity::Module::Module> ListModules() const;

        /**
         * @brief Updates the log level for a single module
         *
         * @param name name of the module
         * @param level log level for the module
         * @return number of modules updated
         */
        long SetModuleLoglevel(const std::string &name, const std::string &level);

        /**
         * @briwf Updates the log level for all modules.
         *
         * @param level log level for the modules
         * @return number of updated modules
         */
        long SetAllModulesLoglevel(const std::string &level);

        /**
         * @brief Sets the log channel and log level for a single module.
         *
         * @param name module name
         * @param channel log channel name
         * @param level log level
         */
        void SetModuleLogChannelAndLevel(const std::string &name, const std::string &channel, const std::string &level);

        /**
         * @brief Deletes module
         *
         * @param module module entity
         */
        void DeleteModule(const Entity::Module::Module &module);

        /**
         * @brief Deletes all modules
         *
         * @return number of modules deleted
         */
        long DeleteAllModules();

    private:
        mutable logger_t _logger{boost::log::keywords::channel = "Module"};

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Module collection name
         */
        std::string _moduleCollectionName;

        /**
         * Modules in-memory database
         */
        ModuleMemoryDb _memoryDb{};

        /**
         * Existing modules
         */
        static std::map<std::string, Entity::Module::Module> _existingModules;
    };

} // namespace AwsMock::Database
