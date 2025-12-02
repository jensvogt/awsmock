//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SSM_MEMORYDB_H
#define AWSMOCK_REPOSITORY_SSM_MEMORYDB_H

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
#include <awsmock/utils/SortColumn.h>
#include <awsmock/core/Linq.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/ssm/Parameter.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::stream::document;

    /**
     * @brief ssm in-memory database
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SSMMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        SSMMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static SSMMemoryDb &instance() {
            static SSMMemoryDb ssmMemoryDb;
            return ssmMemoryDb;
        }

        /**
         * @brief Check existence of a parameter by name
         *
         * @param name parameter name
         * @return true if key already exists
         * @throws DatabaseException
         */
        bool ParameterExists(const std::string &name);

        /**
         * @brief Returns an SMS parameter by primary key
         *
         * @param oid key primary key
         * @return key entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter GetParameterById(const std::string &oid);

        /**
         * @brief Returns an SSM parameter by name
         *
         * @param name parameter name
         * @return key entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter GetParameterByName(const std::string &name);

        /**
         * @brief List all parameters
         *
         * @param region AWS region
         * @return ParameterList
         */
        [[nodiscard]] Entity::SSM::ParameterList ListParameters(const std::string &region = {}) const;

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
        [[nodiscard]] Entity::SSM::ParameterList ListParameterCounters(const std::string &region = {}, const std::string &prefix = {}, long pageSize = -1, long pageIndex = -1, const std::vector<SortColumn> &sortColumns = {});

        /**
         * @brief Returns the total number of parameters
         *
         * @return total number of parameters
         * @throws DatabaseException
         */
        [[nodiscard]] long CountParameters() const;

        /**
         * @brief Create a new parameter in the ssm parameter table
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter CreateParameter(const Entity::SSM::Parameter &parameter);

        /**
         * @brief Updates a parameter
         *
         * @param parameter parameter entity
         * @return created parameter entity
         * @throws DatabaseException
         */
        Entity::SSM::Parameter UpdateParameter(const Entity::SSM::Parameter &parameter);

        /**
         * @brief Deletes a parameter
         *
         * @param parameter parameter entity
         * @throws DatabaseException
         */
        void DeleteParameter(const Entity::SSM::Parameter &parameter);

        /**
         * @brief Deletes all parameters
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        long DeleteAllParameters();

      private:

        /**
         * SSM parameter vector, when running without database
         */
        std::map<std::string, Entity::SSM::Parameter> _parameters{};

        /**
         * Parameter mutex
         */
        static boost::mutex _parameterMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_KMS_DATABASE_H
