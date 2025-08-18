//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_APPLICATION_MEMORYDB_H
#define AWSMOCK_REPOSITORY_APPLICATION_MEMORYDB_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/apps/Application.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    /**
     * @brief Application in-memory database.
     *
     * @par
     * Provides an in-memory database using a simple hash map. The key is a randomly generated UUID.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        ApplicationMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static ApplicationMemoryDb &instance() {
            static ApplicationMemoryDb applicationMemoryDb;
            return applicationMemoryDb;
        }

        /**
         * @brief Check the existence of an application
         *
         * @param region AWS region name
         * @param name application name
         * @return true, if the application exists
         * @throws DatabaseException
         */
        bool ApplicationExists(const std::string &region, const std::string &name);

        /**
         * @brief Returns an application entity by primary key
         *
         * @param oid application primary key
         * @return application entity
         * @throws DatabaseException
         */
        Entity::Apps::Application GetApplicationByOid(const std::string &oid);

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
         * @brief Create a new application
         *
         * @param application application entity to create
         * @return created cognito user pool entity.
         */
        Entity::Apps::Application CreateApplication(const Entity::Apps::Application &application);

        /**
         * @brief Update an application
         *
         * @param application application entity to update
         * @return updated application entity.
         */
        Entity::Apps::Application UpdateApplication(Entity::Apps::Application &application);

        /**
         * @brief Returns a list of cognito user pools.
         *
         * @param region AWS region name
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns vector of sort columns and direction
         * @return list of cognito user pools
         */
        std::vector<Entity::Apps::Application> ListApplications(const std::string &region = {}, const std::string &prefix = {}, long pageSize = -1, long pageIndex = -1, const std::vector<SortColumn> &sortColumns = {});

        /**
         * @brief Count all applications by region and prefix
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of applications
         */
        long CountApplications(const std::string &region, const std::string &prefix) const;

        /**
         * @brief Deletes an application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]] long DeleteApplication(const std::string &region, const std::string &name);


        /**
         * @brief Deletes all application
         *
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]] long DeleteAllApplications();

      private:

        /**
         * Application map
         */
        std::map<std::string, Entity::Apps::Application> _applications{};

        /**
         * Application mutex
         */
        static boost::mutex _applicationMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_APPLICATION_MEMORYDB_H
