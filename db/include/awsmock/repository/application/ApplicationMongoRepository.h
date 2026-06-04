//
// Created by vogje01 on 29/11/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/repository/application/IApplicationRepository.h>
#include <awsmock/utils/ConnectionPool.h>

namespace Awsmock::Database {

    /**
     * @brief Application MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationMongoRepository final : public IApplicationRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit ApplicationMongoRepository() = default;

        /**
         * @brief Check the existence of the application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool ApplicationExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Create a new application
         *
         * @param application application entity to create
         * @return created application entity.
         */
        [[nodiscard]]
        Entity::Apps::Application CreateApplication(Entity::Apps::Application &application) const override;

        /**
         * @brief Update an application
         *
         * @param application application entity to update
         * @return updated application entity.
         */
        [[nodiscard]]
        Entity::Apps::Application UpdateApplication(Entity::Apps::Application &application) const override;

        /**
         * @brief Toggle the enabled flag of an application without loading the full entity.
         *
         * @param region AWS region
         * @param name application name
         * @param enabled new value
         * @throws DatabaseException
         */
        void SetEnabled(const std::string &region, const std::string &name, bool enabled) const override;

        /**
         * @brief Import an application
         *
         * @param application application entity to import
         * @return imported application entity.
         */
        [[nodiscard]]
        Entity::Apps::Application ImportApplication(Entity::Apps::Application &application) const override;

        /**
         * @brief Get an application
         *
         * @param region AWS region
         * @param name application name
         * @return application entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Apps::Application GetApplication(const std::string &region, const std::string &name) const override;

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
        [[nodiscard]]
        std::vector<Entity::Apps::Application> ListApplications(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Count all applications by region and prefix
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of applications
         */
        [[nodiscard]]
        long CountApplications(const std::string &region = {}, const std::string &prefix = {}) const override;

        /**
         * @brief Deletes an application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        long DeleteApplication(const std::string &region, const std::string &name) const override;

        /**
         * @brief Deletes all applications
         *
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        long DeleteAllApplications() const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Application"};

        /**
         * @brief Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * @brief User pool collection name
         */
        static constexpr auto _applicationCollectionName = "application";
    };

}// namespace Awsmock::Database
