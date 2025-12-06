//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SSM_DATABASE_H
#define AWSMOCK_REPOSITORY_SSM_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/ssm/Parameter.h>
#include <awsmock/memorydb/SSMMemoryDb.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    /**
     * @brief ssm MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SSMDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit SSMDatabase() : _databaseName(GetDatabaseName()), _parameterCollectionName("ssm_parameter"), _memoryDb(SSMMemoryDb::instance()) {}

        /**
         * @brief Singleton instance
         */
        static SSMDatabase &instance() {
            static SSMDatabase ssmDatabase;
            return ssmDatabase;
        }

        /**
         * @brief Check existence of a parameter by name
         *
         * @param name parameter name
         * @return true if key already exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool ParameterExists(const std::string &name) const;

        /**
         * @brief Returns an SSM parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter GetParameterById(const std::string &oid) const;

        /**
         * @brief Returns a parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter GetParameterById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a parameter by name
         *
         * @param name parameter name
         * @return key entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter GetParameterByName(const std::string &name) const;

        /**
         * @brief List all parameters
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sort columns list
         * @return ParameterList
         */
        Entity::SSM::ParameterList ListParameters(const std::string &region = {}, const std::string &prefix = {}, long pageSize = -1, long pageIndex = -1, const std::vector<SortColumn> &sortColumns = {}) const;

        /**
         * @brief Returns the total number of parameters
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return total number of parameters
         * @throws DatabaseException
         */
        long CountParameters(const std::string &region = {}, const std::string &prefix = {}) const;

        /**
         * @brief Create a new parameter in the ssm parameter table
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter CreateParameter(Entity::SSM::Parameter &parameter) const;

        /**
         * @brief Updates a parameter
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter UpdateParameter(Entity::SSM::Parameter &parameter) const;

        /**
         * @brief Inserts or updates a parameter
         *
         * @param parameter parameter entity
         * @return inserted or updated parameter entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter ImportParameter(Entity::SSM::Parameter &parameter) const;

        /**
         * @brief Deletes a parameter
         *
         * @param parameter parameter entity
         * @throws DatabaseException
         */
        void DeleteParameter(const Entity::SSM::Parameter &parameter) const;

        /**
         * @brief Delete a all parameters
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        long DeleteAllParameters() const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Parameter collection name
         */
        std::string _parameterCollectionName;

        /**
         * ssm in-memory database
         */
        SSMMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_KMS_DATABASE_H
