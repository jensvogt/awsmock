//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/ssm/Parameter.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for SQS repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * SQS-related data.
     */
    class ISSMRepository {

      public:

        /**
         * @brief Virtual destructor for the ISQSRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~ISSMRepository() = default;

        /**
         * @brief Check the existence of a parameter by name
         *
         * @param name parameter name
         * @return true if the key already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool parameterExists(const std::string &name) const = 0;

        /**
         * @brief Returns an SSM parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SSM::Parameter getParameterById(const std::string &oid) const = 0;

        /**
         * @brief Returns a parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SSM::Parameter getParameterById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns a parameter by name
         *
         * @param name parameter name
         * @return key entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SSM::Parameter getParameterByName(const std::string &name) const = 0;

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
        virtual Entity::SSM::ParameterList listParameters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns the total number of parameters
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return total number of parameters
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long countParameters(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Create a new parameter in the ssm parameter table
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SSM::Parameter createParameter(Entity::SSM::Parameter &parameter) const = 0;

        /**
         * @brief Updates a parameter
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SSM::Parameter updateParameter(Entity::SSM::Parameter &parameter) const = 0;

        /**
         * @brief Inserts or updates a parameter
         *
         * @param parameter parameter entity
         * @return inserted or updated parameter entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SSM::Parameter importParameter(Entity::SSM::Parameter &parameter) const = 0;

        /**
         * @brief Deletes a parameter
         *
         * @param parameter parameter entity
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteParameter(const Entity::SSM::Parameter &parameter) const = 0;

        /**
         * @brief Delete all parameters
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllParameters() const = 0;
    };

}// namespace Awsmock::Database