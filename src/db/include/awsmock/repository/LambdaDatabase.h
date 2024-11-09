//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_LAMBDA_DATABASE_H
#define AWSMOCK_REPOSITORY_LAMBDA_DATABASE_H

// C++ standard includes
#include <iostream>
#include <string>
#include <vector>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include "awsmock/core/exception/DatabaseException.h"
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/memorydb/LambdaMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * Lambda MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit LambdaDatabase();

        /**
         * @brief Singleton instance
         */
        static LambdaDatabase &instance() {
            static LambdaDatabase lambdaDatabase;
            return lambdaDatabase;
        }

        /**
         * @brief Check existence of lambda
         *
         * @param region AWS region name
         * @param function AWS function
         * @param runtime lambda runtime
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        bool LambdaExists(const std::string &region, const std::string &function, const std::string &runtime);

        /**
         * Check existence of lambda
         *
         * @param lambda AWS function
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        bool LambdaExists(const Entity::Lambda::Lambda &lambda);

        /**
         * @brief Check existence of lambda
         *
         * @param function AWS function
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        bool LambdaExists(const std::string &function);

        /**
         * @brief Check existence of lambda
         *
         * @param arn AWS ARN
         * @return true if lambda exists
         * @throws DatabaseException
         */
        bool LambdaExistsByArn(const std::string &arn);

        /**
         * @brief Create a new lambda function
         *
         * @param lambda lambda entity
         * @return created lambda entity.
         */
        Entity::Lambda::Lambda CreateLambda(const Entity::Lambda::Lambda &lambda);

        /**
         * @brief Count all lambdas
         *
         * @param region aws-mock region.
         * @return total number of lambdas.
         */
        int LambdaCount(const std::string &region = {});

        /**
         * @brief Updates an existing lambda lambda function
         *
         * @param lambda lambda entity
         * @return updated lambda entity.
         */
        Entity::Lambda::Lambda UpdateLambda(const Entity::Lambda::Lambda &lambda);

        /**
         * @brief Created or updates an existing lambda function
         *
         * @param lambda lambda entity
         * @return created or updated lambda entity.
         */
        Entity::Lambda::Lambda CreateOrUpdateLambda(const Entity::Lambda::Lambda &lambda);

        /**
         * @brief Returns a lambda entity by primary key
         *
         * @param oid lambda primary key
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaById(bsoncxx::oid oid);

        /**
         * @brief Returns a lambda entity by primary key
         *
         * @param oid lambda primary key
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaById(const std::string &oid);

        /**
         * @brief Returns a lambda entity by ARN
         *
         * @param arn lambda ARN
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaByArn(const std::string &arn);

        /**
         * @brief Returns a lambda entity by name
         *
         * @param region AWS region
         * @param name function name
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaByName(const std::string &region, const std::string &name);

        /**
         * @brief Sets the status of an lambda instance
         *
         * @param containerId lambda container ID
         * @param status lambda instance status
         * @throws DatabaseException
         */
        void SetInstanceStatus(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status);

        /**
         * @brief Sets the average runtime of a lambda instance
         *
         * @param oid lambda ID
         * @param millis lambda invocation runtime
         * @throws DatabaseException
         */
        void SetAverageRuntime(const std::string &oid, long millis);

        /**
         * @brief Returns a list of lambda functions.
         *
         * @param region AWS region name
         * @return list of lambda functions
         */
        std::vector<Entity::Lambda::Lambda> ListLambdas(const std::string &region = {});

        /**
         * @brief Returns a list of lambda functions.
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param maxResult maximal number of results
         * @param skip number of records to skip
         * @param sortColumns sorting columns
         * @return list of lambda function counters
         */
        std::vector<Entity::Lambda::Lambda> ListLambdaCounters(const std::string &region = {}, const std::string &prefix = {}, long maxResults = 0, long skip = 0, const std::vector<Core::SortColumn> &sortColumns = {});

        /**
         * @brief Returns a list of lambda functions with the given event source ARN attached.
         *
         * @param eventSourceArn event source ARN
         * @return list of lambda functions
         */
        std::vector<Entity::Lambda::Lambda> ListLambdasWithEventSource(const std::string &eventSourceArn);

        /**
         * @brief Deletes an existing lambda function
         *
         * @param functionName lambda function name
         * @throws DatabaseException
         */
        void DeleteLambda(const std::string &functionName);

        /**
         * @brief Deletes all existing lambda functions
         *
         * @throws DatabaseException
         */
        void DeleteAllLambdas();

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Collection name
         */
        std::string _collectionName;

        /**
         * Lambda in-memory database
         */
        LambdaMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_LAMBDA_DATABASE_H
