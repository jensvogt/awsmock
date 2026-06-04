//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include "ISSMRepository.h"


#include <awsmock/core/Linq.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/ssm/Parameter.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief ssm in-memory database
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SSMMemoryRepository final : public ISSMRepository {

      public:

        /**
         * @brief Constructor
         */
        SSMMemoryRepository() = default;

        /**
         * @brief Singleton instance
         */
        static SSMMemoryRepository &instance() {
            static SSMMemoryRepository instance;
            return instance;
        }

        /**
         * @brief Check the existence of a parameter by name
         *
         * @param name parameter name
         * @return true if the key already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool parameterExists(const std::string &name) const override;

        /**
         * @brief Returns an SMS parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter getParameterById(const std::string &oid) const override;

        /**
         * @brief Returns an SMS parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter getParameterById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns an SSM parameter by name
         *
         * @param name parameter name
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter getParameterByName(const std::string &name) const override;

        /**
         * @brief List all parameters
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sort column list
         * @return ParameterList
         */
        [[nodiscard]]
        Entity::SSM::ParameterList listParameters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns the total number of parameters
         *
         * @return total number of parameters
         * @throws DatabaseException
         */
        [[nodiscard]]
        long countParameters(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Create a new parameter in the ssm parameter table
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter createParameter(Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Updates a parameter
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter updateParameter(Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Import parameters
         *
         * @param parameter parameter entity
         * @return inserted or updated parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SSM::Parameter importParameter(Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Deletes a parameter
         *
         * @param parameter parameter entity
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteParameter(const Entity::SSM::Parameter &parameter) const override;

        /**
         * @brief Deletes all parameters
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllParameters() const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SSM"};

        /**
         * @brief SSM parameter vector when running without a database
         */
        mutable std::map<std::string, Entity::SSM::Parameter> _parameters{};

        /**
         * @brief Parameter mutex
         */
        static boost::mutex _parameterMutex;
    };

}// namespace Awsmock::Database
