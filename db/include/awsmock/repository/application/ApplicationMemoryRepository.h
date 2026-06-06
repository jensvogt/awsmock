//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/Linq.h>
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/PagingUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/apps/Application.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/application/IApplicationRepository.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Application in-memory database.
     *
     * @par
     * Provides an in-memory database using a simple hash map. The key is a randomly generated UUID.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApplicationMemoryRepository final : public IApplicationRepository {

    public:
        /**
         * @brief Constructor
         */
        ApplicationMemoryRepository() = default;

        /**
         * @brief Check the existence of an application
         *
         * @param region AWS region name
         * @param name application name
         * @return true, if the application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool applicationExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Get an application
         *
         * @param region AWS region
         * @param name application name
         * @return application entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Apps::Application getApplication(const std::string &region, const std::string &name) const override;

        /**
         * @brief Create a new application
         *
         * @param application application entity to create
         * @return created cognito user pool entity.
         */
        [[nodiscard]]
        Entity::Apps::Application createApplication(Entity::Apps::Application &application) const override;

        /**
         * @brief Update an application
         *
         * @param application application entity to update
         * @return updated application entity.
         */
        [[nodiscard]]
        Entity::Apps::Application updateApplication(Entity::Apps::Application &application) const override;

        /**
         * @brief Toggle the enabled flag of an application without loading the full entity.
         *
         * @param region AWS region
         * @param name application name
         * @param enabled new value
         * @throws DatabaseException
         */
        void setEnabled(const std::string &region, const std::string &name, bool enabled) const override;

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
        [[nodiscard]]
        std::vector<Entity::Apps::Application> listApplications(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Count all applications by region and prefix
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of applications
         */
        [[nodiscard]]
        long countApplications(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Import an application
         *
         * @param application application entity to import
         * @return imported application entity.
         */
        [[nodiscard]]
        Entity::Apps::Application importApplication(Entity::Apps::Application &application) const override;

        /**
         * @brief Deletes an application
         *
         * @param region AWS region name
         * @param name application name
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteApplication(const std::string &region, const std::string &name) const override;

        /**
         * @brief Deletes all applications
         *
         * @return true if, application exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllApplications() const override;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Application"};

        /**
         * Application map
         */
        mutable std::unordered_map<std::string, Entity::Apps::Application> _applications{};

        /**
         * Application mutex
         */
        static boost::mutex _applicationMutex;
    };

} // namespace Awsmock::Database
