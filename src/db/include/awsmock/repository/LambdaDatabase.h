//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_LAMBDA_DATABASE_H
#define AWSMOCK_REPOSITORY_LAMBDA_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/entity/lambda/LambdaResult.h>
#include <awsmock/memorydb/LambdaMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

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
         * @brief Check the existence of lambda
         *
         * @param region AWS region name
         * @param function AWS function
         * @param runtime lambda runtime
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        bool LambdaExists(const std::string &region, const std::string &function, const std::string &runtime) const;

        /**
         * Check the existence of lambda
         *
         * @param lambda AWS function
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        bool LambdaExists(const Entity::Lambda::Lambda &lambda) const;

        /**
         * @brief Check the existence of lambda
         *
         * @param functionName AWS function
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        bool LambdaExists(const std::string &functionName) const;

        /**
         * @brief Check the existence of lambda
         *
         * @param arn AWS ARN
         * @return true if lambda exists
         * @throws DatabaseException
         */
        bool LambdaExistsByArn(const std::string &arn) const;

        /**
         * @brief Create a new lambda function
         *
         * @param lambda lambda entity
         * @return created lambda entity.
         */
        Entity::Lambda::Lambda CreateLambda(const Entity::Lambda::Lambda &lambda) const;

        /**
         * @brief Count all lambdas
         *
         * @param region aws-mock region.
         * @return total number of lambdas.
         */
        int LambdaCount(const std::string &region = {}) const;

        /**
         * @brief Updates an existing lambda lambda function
         *
         * @param lambda lambda entity
         * @return updated lambda entity.
         */
        Entity::Lambda::Lambda UpdateLambda(const Entity::Lambda::Lambda &lambda) const;

        /**
         * @brief Created or updates an existing lambda function
         *
         * @param lambda lambda entity
         * @return created or updated lambda entity.
         */
        Entity::Lambda::Lambda CreateOrUpdateLambda(const Entity::Lambda::Lambda &lambda) const;

        /**
         * @brief Import a lambda function
         *
         * @param lambda lambda entity
         * @return imported lambda entity.
         */
        Entity::Lambda::Lambda ImportLambda(Entity::Lambda::Lambda &lambda) const;

        /**
         * @brief Returns a lambda entity by primary key
         *
         * @param oid lambda primary key
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a lambda entity by primary key
         *
         * @param oid lambda primary key
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaById(const std::string &oid) const;

        /**
         * @brief Returns a lambda entity by ARN
         *
         * @param arn lambda ARN
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaByArn(const std::string &arn) const;

        /**
         * @brief Returns a lambda entity by name
         *
         * @param region AWS region
         * @param name function name
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaByName(const std::string &region, const std::string &name) const;

        /**
         * @brief Sets the status of a lambda instance
         *
         * @param containerId lambda container ID
         * @param status lambda instance status
         * @throws DatabaseException
         */
        void SetInstanceStatus(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status) const;

        /**
         * @brief Sets the average runtime of a lambda instance
         *
         * @param oid lambda ID
         * @param timestamp last update timestamp
         * @throws DatabaseException
         */
        void SetLastInvocation(const std::string &oid, const system_clock::time_point &timestamp) const;

        /**
         * @brief Sets the average runtime of a lambda instance
         *
         * @param oid lambda ID
         * @param millis lambda invocation runtime
         * @throws DatabaseException
         */
        void SetAverageRuntime(const std::string &oid, long millis) const;

        /**
         * @brief Returns a list of lambda functions.
         *
         * @param region AWS region name
         * @return list of lambda functions
         */
        [[nodiscard]] std::vector<Entity::Lambda::Lambda> ListLambdas(const std::string &region = {}) const;

        /**
         * @brief Returns a list of lambda functions.
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param maxResults maximal number of results
         * @param skip number of records to skip
         * @param sortColumns sorting columns
         * @return list of lambda function counters
         */
        [[nodiscard]] std::vector<Entity::Lambda::Lambda> ListLambdaCounters(const std::string &region = {}, const std::string &prefix = {}, long maxResults = 0, long skip = 0, const std::vector<SortColumn> &sortColumns = {}) const;

        /**
         * @brief Export a list of lambdas
         *
         * @param sortColumns sorting columns
         * @return ölist of lambda entries
         */
        [[nodiscard]] std::vector<Entity::Lambda::Lambda> ExportLambdas(const std::vector<SortColumn> &sortColumns = {}) const;

        /**
         * @brief Returns a list of lambda functions with the given event source ARN attached.
         *
         * @param eventSourceArn event source ARN
         * @return list of lambda functions
         */
        [[nodiscard]] std::vector<Entity::Lambda::Lambda> ListLambdasWithEventSource(const std::string &eventSourceArn) const;

        /**
         * @brief Creates a new lambda result
         *
         * @param lambdaResult lambda result record
         * @return created lambdaResult entity
         */
        Entity::Lambda::LambdaResult CreateLambdaResult(Entity::Lambda::LambdaResult &lambdaResult) const;

        /**
         * @brief Removes old lambda logs
         *
         * @param cutOff cut off time point
         * @return numberof logs removed
         */
        [[nodiscard]] long RemoveExpiredLambdaLogs(const system_clock::time_point &cutOff) const;

        /**
         * @brief Returns a lambda function result.
         *
         * @param oid lambda result OID
         * @return lambda function result counter
         */
        [[nodiscard]] Entity::Lambda::LambdaResult GetLambdaResultCounter(const std::string &oid) const;

        /**
         * @brief Returns a list of lambda function results.
         *
         * @param lambdaArn lambda function ARN
         * @param prefix name prefix
         * @param pageSize maximal number of results
         * @param pageIndex number of records to skip
         * @param sortColumns sorting columns
         * @return list of lambda function result counters
         */
        [[nodiscard]] std::vector<Entity::Lambda::LambdaResult> ListLambdaResultCounters(const std::string &lambdaArn, const std::string &prefix = {}, long pageSize = 0, long pageIndex = 0, const std::vector<SortColumn> &sortColumns = {}) const;

        /**
         * @brief Returns the total number of lambda results
         *
         * @param lambdaArn lambda function ARN
         * @return total number of results
         */
        [[nodiscard]] long LambdaResultsCount(const std::string &lambdaArn) const;

        /**
         * @brief Deletes a lambda result counter
         *
         * @param oid lambda function oid
         * @return number of results deleted
         */
        long DeleteResultsCounter(const std::string &oid) const;

        /**
         * @brief Deletes all lambda result counter for a lambda function
         *
         * @param lambdaArn lambda function ARN
         * @return number of results deleted
         */
        long DeleteResultsCounters(const std::string &lambdaArn) const;

        /**
         * @brief Deletes an existing lambda function
         *
         * @param functionName lambda function name
         * @throws DatabaseException
         */
        void DeleteLambda(const std::string &functionName) const;

        /**
         * @brief Deletes all existing lambda functions
         *
         * @return number of lambda object deleted
         * @throws DatabaseException
         */
        [[nodiscard]] long DeleteAllLambdas() const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Lambda collection name
         */
        std::string _lambdaCollectionName;

        /**
         * Lambda result collection name
         */
        std::string _lambdaResultCollectionName;

        /**
         * Lambda in-memory database
         */
        LambdaMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_LAMBDA_DATABASE_H
