//
// Created by vogje01 on 29/11/2023.
//

#ifndef AWSMOCK_REPOSITORY_APPLICATION_DATABASE_H
#define AWSMOCK_REPOSITORY_APPLICATION_DATABASE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/memorydb/ApplicationMemoryDb.h>
#include <awsmock/memorydb/CognitoMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    /**
     * @brief Application MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit ApplicationDatabase() : _databaseName(GetDatabaseName()), _applicationCollectionName("apps_application"), _memoryDb(ApplicationMemoryDb::instance()) {};

        /**
         * @brief Singleton instance
         */
        static ApplicationDatabase &instance() {
            static ApplicationDatabase applicationDatabase;
            return applicationDatabase;
        }

        /**
         * @brief Check existence of application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool ApplicationExists(const std::string &region, const std::string &name) const;

        /**
         * @brief Create a new application
         *
         * @param application application entity to create
         * @return created application entity.
         */
        Entity::Apps::Application CreateApplication(Entity::Apps::Application &application) const;

        /**
         * @brief Update an application
         *
         * @param application application entity to update
         * @return updated application entity.
         */
        Entity::Apps::Application UpdateApplication(Entity::Apps::Application &application) const;

        /**
         * @brief Import an application
         *
         * @param application application entity to import
         * @return imported application entity.
         */
        Entity::Apps::Application ImportApplication(Entity::Apps::Application &application) const;

        /**
         * @brief Get an application
         *
         * @param region AWS region
         * @param name application name
         * @return application entity
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::Apps::Application GetApplication(const std::string &region, const std::string &name) const;

        /**
         * @brief Returns a list of applications
         *
         * @param region AWS region name
         * @param prefix name prtefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns vector of sort columns and direction
         * @return list of applications
         */
        std::vector<Entity::Apps::Application> ListApplications(const std::string &region = {}, const std::string &prefix = {}, long pageSize = -1, long pageIndex = -1, const std::vector<SortColumn> &sortColumns = {}) const;

        /**
         * @brief Count all applications by region and prefix
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of applications
         */
        long CountApplications(const std::string &region = {}, const std::string &prefix = {}) const;

        /**
         * @brief Deletes an application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]] long DeleteApplication(const std::string &region, const std::string &name) const;


        /**
         * @brief Deletes all application
         *
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]] long DeleteAllApplications() const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Application collection name
         */
        std::string _applicationCollectionName;

        /**
         * Application in-memory database
         */
        ApplicationMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_COGNITO_DATABASE_H
