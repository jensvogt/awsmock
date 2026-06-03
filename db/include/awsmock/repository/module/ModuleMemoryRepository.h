//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include "IModuleRepository.h"


#include <awsmock/core/StringUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/Database.h>

namespace Awsmock::Database {

    /**
     * @brief Module in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ModuleMemoryRepository final : public IModuleRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit ModuleMemoryRepository() = default;

        /**
         * @brief Singleton instance
         *
         * @return singleton instance
         */
        static ModuleMemoryRepository &instance() {
            static ModuleMemoryRepository moduleMemoryDb;
            return moduleMemoryDb;
        }

        /**
         * @brief Initialize the database
         */
        void initialize() const override;

        /**
         * @brief Checks the active flag.
         *
         * @param name module name
         * @return true if active
         */
        bool isActive(const std::string &name) const override;

        /**
         * @brief Check the existence of a module
         *
         * @param name module name
         * @return created name
         */
        bool moduleExists(const std::string &name) const override;

        /**
         * @brief Returns the module by ID
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        Entity::Module::Module getModuleById(const std::string &oid) const override;

        /**
         * @brief Returns the module OID
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        Entity::Module::Module getModuleById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns the module by userPoolId
         *
         * @param name module name
         * @return module, if existing
         * @throws DatabaseException
         */
        Entity::Module::Module getModuleByName(const std::string &name) const override;

        /**
         * @brief Returns all module names
         *
         * @return list of module names
         * @throws DatabaseException
         */
        std::vector<std::string> getAllModuleNames() const override;

        /**
         * @brief Returns all module state
         *
         * @return module state
         * @throws DatabaseException
         */
        Entity::Module::ModuleState getState(const std::string &name) const override;

        /**
         * @brief Creates a module
         *
         * @param module module entity
         * @return created module
         */
        Entity::Module::Module createModule(Entity::Module::Module &module) const override;

        /**
         * @brief Creates a module
         *
         * @param module module entity
         * @return created module
         */
        Entity::Module::Module createOrUpdateModule(Entity::Module::Module &module) const override;

        /**
         * @brief Updates a module
         *
         * @param module module entity
         * @return updated module
         */
        [[nodiscard]]
        Entity::Module::Module updateModule(Entity::Module::Module &module) const override;

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
        void setState(const std::string &name, const Entity::Module::ModuleState &state) const override;

        /**
         * @brief Sets the status of module.
         *
         * <p>Status can be one of UNKNOWN, ACTIVE, INACTIVE.</p>
         *
         * @param name module name
         * @param status module status
         * @see AwsMock::Database::Entity::Module::ModuleStatus()
         */
        void setStatus(const std::string &name, const Entity::Module::ModuleStatus &status) const override;

        /**
         * @brief Sets the port of module.
         *
         * @param name module name
         * @param port module port
         */
        void setModulePort(const std::string &name, int port) const override;

        /**
         * @brief Counts the number of modules
         *
         * @return total number of modules
         */
        [[nodiscard]]
        int moduleCount() const override;

        /**
         * @brief Returns a list of all modules
         *
         * @return list of all modules
         */
        [[nodiscard]]
        std::vector<Entity::Module::Module> listModules() const override;

        /**
         * @brief Updates the log level for a single module
         *
         * @param name name of the module
         * @param level log level for the module
         * @return number of modules updated
         */
        long setModuleLoglevel(const std::string &name, const std::string &level) const override;

        /**
         * @briwf Updates the log level for all modules.
         *
         * @param level log level for the modules
         * @return number of updated modules
         */
        long setAllModulesLoglevel(const std::string &level) const override;

        /**
         * @brief Sets the log channel and log level for a single module.
         *
         * @param name module name
         * @param channel log channel name
         * @param level log level
         */
        void setModuleLogChannelAndLevel(const std::string &name, const std::string &channel, const std::string &level) const override;

        /**
         * @brief Deletes module
         *
         * @param module module entity
         */
        void deleteModule(const Entity::Module::Module &module) const override;

        /**
         * @brief Deletes all modules
         *
         * @return number of modules deleted
         */
        long deleteAllModules() const override;

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Module"};

        /**
         * Module map
         */
        mutable std::map<std::string, Entity::Module::Module> _modules{};

        /**
         * Existing modules
         */
        static std::map<std::string, Entity::Module::Module> _existingModules;

        /**
         * Module lock
         */
        static boost::mutex _moduleMutex;
    };

}// namespace Awsmock::Database
