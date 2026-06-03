//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/module/Module.h>
#include <awsmock/repository/DatabaseBase.h>
#include <awsmock/repository/module/IModuleRepository.h>
#include <awsmock/repository/module/ModuleMemoryRepository.h>

namespace Awsmock::Database {

    /**
     * @brief Module MongoDB database.
     *
     * Controls all the AwsMock modules.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ModuleMongoRepository final : public IModuleRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit ModuleMongoRepository() = default;

        /**
         * @brief Singleton instance
         */
        static ModuleMongoRepository &instance() {
            static ModuleMongoRepository instance;
            return instance;
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
         * @par
         * This method checks only the name of the module
         *
         * @param name module name
         * @return created name
         */
        bool moduleExists(const std::string &name) const override;

        /**
         * @brief Returns the module by OID
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Module::Module getModuleById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns the module by OID as string value
         *
         * @param oid module oid
         * @return module, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Module::Module getModuleById(const std::string &oid) const override;

        /**
         * @brief Returns the module by name
         *
         * @param name module name
         * @return module, if existing
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Module::Module getModuleByName(const std::string &name) const override;

        /**
         * @brief Returns all module names
         *
         * @return list of module names
         * @throws DatabaseException
         */
        [[nodiscard]]
        std::vector<std::string> getAllModuleNames() const override;

        /**
         * @brief Creates a new module
         *
         * @param module module entity
         * @return created module
         */
        [[nodiscard]]
        Entity::Module::Module createModule(Entity::Module::Module &module) const override;

        /**
         * @brief Updates an existing module
         *
         * @param module module entity
         * @return updated module
         */
        [[nodiscard]]
        Entity::Module::Module updateModule(Entity::Module::Module &module) const override;

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
        void setState(const std::string &name, const Entity::Module::ModuleState &state) const override;

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
        Entity::Module::ModuleState getState(const std::string &name) const override;

        /**
         * @brief Sets the status of a module.
         *
         * <p>Status can be one of UNKNOWN, ACTIVE, INACTIVE.</p>
         *
         * @param name module name
         * @param status module status
         * @see AwsMock::Database::Entity::Module::ModuleStatus()
         */
        void setStatus(const std::string &name, const Entity::Module::ModuleStatus &status) const override;

        /**
         * @brief Sets the port of a module.
         *
         * @param name module name
         * @param port module port
         */
        void setModulePort(const std::string &name, int port) const override;

        /**
         * @brief Creates or updates a module
         *
         * @param modules module entity
         * @return updated modules
         */
        [[nodiscard]]
        Entity::Module::Module createOrUpdateModule(Entity::Module::Module &modules) const override;

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
        [[nodiscard]]
        long setModuleLoglevel(const std::string &name, const std::string &level) const override;

        /**
         * @briwf Updates the log level for all modules.
         *
         * @param level log level for the modules
         * @return number of updated modules
         */
        [[nodiscard]]
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
        [[nodiscard]]
        long deleteAllModules() const override;

        /**
         * @brief Returns a list of existing modules.
         *
         * @return map of existing modules
         */
        static std::map<std::string, Entity::Module::Module> getExisting();

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Module"};

        /**
         * Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * Module collection name
         */
        static constexpr auto _moduleCollectionName = "module";

        /**
         * Existing modules
         */
        static std::map<std::string, Entity::Module::Module> _existingModules;
    };

}// namespace Awsmock::Database
