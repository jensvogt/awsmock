//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/module/Module.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for SQS repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * SQS-related data.
     */
    class IModuleRepository {

      public:

        /**
         * @brief Virtual destructor for the module repository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~IModuleRepository() = default;

        /**
         * @brief Initialize the database
         */
        virtual void initialize() const = 0;

        /**
         * @brief Checks the active flag.
         *
         * @param name module name
         * @return true if active
         */
        virtual bool isActive(const std::string &name) const = 0;

        /**
         * @brief Check the existence of a module
         *
         * @par
         * This method checks only the name of the module
         *
         * @param name module name
         * @return created name
         */
        virtual bool moduleExists(const std::string &name) const = 0;

        /**
         * @brief Returns the module by OID
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Module::Module getModuleById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns the module by OID as string value
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Module::Module getModuleById(const std::string &oid) const = 0;

        /**
         * @brief Returns the module by name
         *
         * @param name module name
         * @return module, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Module::Module getModuleByName(const std::string &name) const = 0;

        /**
         * @brief Returns all module names
         *
         * @return list of module names
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual std::vector<std::string> getAllModuleNames() const = 0;

        /**
         * @brief Creates a new module
         *
         * @param module module entity
         * @return created module
         */
        [[nodiscard]]
        virtual Entity::Module::Module createModule(Entity::Module::Module &module) const = 0;

        /**
         * @brief Updates an existing module
         *
         * @param module module entity
         * @return updated module
         */
        [[nodiscard]]
        virtual Entity::Module::Module updateModule(Entity::Module::Module &module) const = 0;

        /**
         * @brief Sets the state of a module.
         *
         * <p>State can be one of STARTING, RUNNING, STOPPED, UNKNOWN.</p>
         *
         * @param name module name
         * @param state module state
         * @see AwsMock::Database::Entity::Module::ModuleState()
         */
        virtual void setState(const std::string &name, const Entity::Module::ModuleState &state) const = 0;

        /**
         * @brief Sets the state of a module.
         *
         * <p>State can be one of STARTING, RUNNING, STOPPED, UNKNOWN.</p>
         *
         * @param name module name
         * @return module state
         * @see AwsMock::Database::Entity::Module::ModuleState()
         */
        [[nodiscard]]
        virtual Entity::Module::ModuleState getState(const std::string &name) const = 0;

        /**
         * @brief Sets the status of a module.
         *
         * <p>Status can be one of UNKNOWN, ACTIVE, INACTIVE.</p>
         *
         * @param name module name
         * @param status module status
         * @see AwsMock::Database::Entity::Module::ModuleStatus()
         */
        virtual void setStatus(const std::string &name, const Entity::Module::ModuleStatus &status) const = 0;

        /**
         * @brief Sets the port of a module.
         *
         * @param name module name
         * @param port module port
         */
        virtual void setModulePort(const std::string &name, int port) const = 0;

        /**
         * @brief Creates or updates a module
         *
         * @param modules module entity
         * @return updated modules
         */
        [[nodiscard]]
        virtual Entity::Module::Module createOrUpdateModule(Entity::Module::Module &modules) const = 0;

        /**
         * @brief Counts the number of modules
         *
         * @return total number of modules
         */
        [[nodiscard]]
        virtual int moduleCount() const = 0;

        /**
         * @brief Returns a list of all modules
         *
         * @return list of all modules
         */
        [[nodiscard]]
        virtual std::vector<Entity::Module::Module> listModules() const = 0;

        /**
         * @brief Updates the log level for a single module
         *
         * @param name name of the module
         * @param level log level for the module
         * @return number of modules updated
         */
        [[nodiscard]]
        virtual long setModuleLoglevel(const std::string &name, const std::string &level) const = 0;

        /**
         * @briwf Updates the log level for all modules.
         *
         * @param level log level for the modules
         * @return number of updated modules
         */
        [[nodiscard]]
        virtual long setAllModulesLoglevel(const std::string &level) const = 0;

        /**
         * @brief Sets the log channel and log level for a single module.
         *
         * @param name module name
         * @param channel log channel name
         * @param level log level
         */
        virtual void setModuleLogChannelAndLevel(const std::string &name, const std::string &channel, const std::string &level) const = 0;

        /**
         * @brief Deletes module
         *
         * @param module module entity
         */
        virtual void deleteModule(const Entity::Module::Module &module) const = 0;

        /**
         * @brief Deletes all modules
         *
         * @return number of modules deleted
         */
        [[nodiscard]]
        virtual long deleteAllModules() const = 0;
    };

}// namespace Awsmock::Database