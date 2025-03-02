//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_MODULE_MEMORYDB_H
#define AWSMOCK_REPOSITORY_MODULE_MEMORYDB_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Macros.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief Module in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ModuleMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        explicit AWSMOCK_API ModuleMemoryDb() = default;

        /**
         * @brief Singleton instance
         *
         * @return singleton instance
         */
        static ModuleMemoryDb &instance() {
            static ModuleMemoryDb moduleMemoryDb;
            return moduleMemoryDb;
        }

        /**
         * @brief Checks the active flag.
         *
         * @param name module name
         * @return true if active
         */
        AWSMOCK_API bool IsActive(const std::string &name);

        /**
         * @brief Check existence of name
         *
         * @param name name name
         * @return created name
         */
        AWSMOCK_API bool ModuleExists(const std::string &name);

        /**
         * @brief Returns the module by userPoolId
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::Module::Module GetModuleById(const std::string &oid);

        /**
         * @brief Returns the module by userPoolId
         *
         * @param name module name
         * @return module, if existing
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::Module::Module GetModuleByName(const std::string &name);

        /**
         * @brief Returns all module names
         *
         * @return list of module names
         * @throws DatabaseException
         */
        AWSMOCK_API std::vector<std::string> GetAllModuleNames();

        /**
         * @brief Creates a module
         *
         * @param module module entity
         * @return created module
         */
        AWSMOCK_API Entity::Module::Module CreateModule(const Entity::Module::Module &module);

        /**
         * @brief Updates a module
         *
         * @param module module entity
         * @return updated module
         */
        AWSMOCK_API Entity::Module::Module UpdateModule(const Entity::Module::Module &module);

        /**
         * @brief Sets the state of module.
         *
         * <p>State can be one of STARTING, RUNNING, STOPPED, UNKNOWN.</p>
         *
         * @param name module name
         * @param state module state
         * @return updated module
         * @see AwsMock::Database::Entity::Module::ModuleState()
         */
        AWSMOCK_API Entity::Module::Module SetState(const std::string &name, const Entity::Module::ModuleState &state);

        /**
         * @brief Sets the status of module.
         *
         * <p>Status can be one of UNKNOWN, ACTIVE, INACTIVE.</p>
         *
         * @param name module name
         * @param status module status
         * @see AwsMock::Database::Entity::Module::ModuleStatus()
         */
        AWSMOCK_API void SetStatus(const std::string &name, const Entity::Module::ModuleStatus &status);

        /**
         * @brief Sets the port of module.
         *
         * @param name module name
         * @param port module port
         */
        AWSMOCK_API void SetPort(const std::string &name, int port);

        /**
         * @brief Counts the number of modules
         *
         * @return total number of modules
         */
        AWSMOCK_API int ModuleCount() const;

        /**
         * @brief Returns a list of all modules
         *
         * @return list of all modules
         */
        AWSMOCK_API Entity::Module::ModuleList ListModules() const;

        /**
         * @brief Deletes module
         *
         * @param module module entity
         */
        AWSMOCK_API void DeleteModule(const Entity::Module::Module &module);

        /**
         * @brief Deletes all modules
         */
        AWSMOCK_API void DeleteAllModules();

      private:

        /**
         * Modules map
         */
        std::map<std::string, Entity::Module::Module> _modules;

        /**
         * Existing modules
         */
        static std::map<std::string, Entity::Module::Module> _existingModules;

        /**
         * Module lock
         */
        static boost::mutex _moduleMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_MODULE_MEMORYDB_H
