//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/apps/Application.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for application repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * Cognito-related data.
     */
    class IApplicationRepository {

      public:

        /**
         * @brief Virtual destructor for the IApplicationRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~IApplicationRepository() = default;

        /**
         * @brief Check the existence of the application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool applicationExists(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Create a new application
         *
         * @param application application entity to create
         * @return created application entity.
         */
        [[nodiscard]]
        virtual Entity::Apps::Application createApplication(Entity::Apps::Application &application) const = 0;

        /**
         * @brief Update an application
         *
         * @param application application entity to update
         * @return updated application entity.
         */
        [[nodiscard]]
        virtual Entity::Apps::Application updateApplication(Entity::Apps::Application &application) const = 0;

        /**
         * @brief Toggle the enabled flag of an application without loading the full entity.
         *
         * @param region AWS region
         * @param name application name
         * @param enabled new value
         * @throws DatabaseException
         */
        virtual void setEnabled(const std::string &region, const std::string &name, bool enabled) const = 0;

        /**
         * @brief Import an application
         *
         * @param application application entity to import
         * @return imported application entity.
         */
        [[nodiscard]]
        virtual Entity::Apps::Application importApplication(Entity::Apps::Application &application) const = 0;

        /**
         * @brief Get an application
         *
         * @param region AWS region
         * @param name application name
         * @return application entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Apps::Application getApplication(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Returns a list of applications
         *
         * @param region AWS region name
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns vector of sort columns and direction
         * @return list of applications
         */
        [[nodiscard]]
        virtual std::vector<Entity::Apps::Application> listApplications(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Count all applications by region and prefix
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of applications
         */
        [[nodiscard]]
        virtual long countApplications(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Deletes an application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteApplication(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Deletes all applications
         *
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllApplications() const = 0;
    };

}// namespace Awsmock::Database