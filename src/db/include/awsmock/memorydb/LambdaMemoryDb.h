//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_LAMBDA_MEMORYDB_H
#define AWSMOCK_REPOSITORY_LAMBDA_MEMORYDB_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/Linq.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/entity/lambda/LambdaResult.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * Lambda in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        LambdaMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static LambdaMemoryDb &instance() {
            static LambdaMemoryDb lambdaMemoryDb;
            return lambdaMemoryDb;
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
         * @brief Check existence of lambda
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
         * Check the existence of lambda
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
         * @brief Returns a lambda entity by primary key
         *
         * @param oid lambda primary key
         * @return lambda entity
         * @throws DatabaseException
         */
        Entity::Lambda::Lambda GetLambdaById(const std::string &oid);

        /**
          * Returns a lambda entity by ARN
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
         * Count all lambdas
         *
         * @param region aws-mock region.
         * @return total number of lambdas.
         */
        long LambdaCount(const std::string &region = {}) const;

        /**
         * @brief Sets the average runtime of a lambda instance
         *
         * @param oid lambda ID
         * @param timestamp last update timestamp
         * @throws DatabaseException
         */
        void SetLastInvocation(const std::string &oid, const system_clock::time_point &timestamp);

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
        std::vector<Entity::Lambda::Lambda> ListLambdas(const std::string &region);

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
        [[nodiscard]] std::vector<Entity::Lambda::Lambda> ListLambdaCounters(const std::string &region = {}, const std::string &prefix = {}, long maxResults = 0, long skip = 0, const std::vector<SortColumn> &sortColumns = {});

        /**
         * @brief Returns a list of lambda functions with the given event source ARN attached.
         *
         * @param eventSourceArn event source ARN
         * @return list of lambda functions
         */
        std::vector<Entity::Lambda::Lambda> ListLambdasWithEventSource(const std::string &eventSourceArn);

        /**
         * @brief Updates an existing lambda lambda function
         *
         * @param lambda lambda entity
         * @return updated lambda entity.
         */
        Entity::Lambda::Lambda UpdateLambda(const Entity::Lambda::Lambda &lambda);

        /**
         * @brief Sets the status of a lambda instance
         *
         * @param containerId lambda container ID
         * @param status lambda instance status
         * @throws DatabaseException
         */
        void SetInstanceValues(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status);

        /**
         * @brief Sets the status of a lambda instance
         *
         * @param lambda lambda function
         * @param invocations number of invocations
         * @param avgRuntime average runtime
         * @throws DatabaseException
         */
        void SetLambdaValues(const Entity::Lambda::Lambda &lambda, long invocations, long avgRuntime);

        /**
         * @brief Creates a new lambda result
         *
         * @param lambdaResult lambda result record
         * @return created lambdaResult entity
         */
        Entity::Lambda::LambdaResult CreateLambdaResult(const Entity::Lambda::LambdaResult &lambdaResult);

        /**
         * @brief Returns the existence of a lambda result
         *
         * @param oid lambda result record OID
         * @return true if the result with the given OID exists
         */
        bool LambdaResultExists(const std::string &oid);

        /**
         * @brief Returns a lambda result by OID
         *
         * @param oid lambda result OID
         * @return lambdaResult entity
         */
        Entity::Lambda::LambdaResult GetLambdaResultById(const std::string &oid);

        /**
         * @brief Returns a list of lambda function results.
         *
         * @param lambdaArn lambda function ARN
         * @param prefix name prefix
         * @param maxResults maximal number of results
         * @param skip number of records to skip
         * @param sortColumns sorting columns
         * @return list of lambda function result counters
         */
        std::vector<Entity::Lambda::LambdaResult> ListLambdaResultCounters(const std::string &lambdaArn, const std::string &prefix = {}, long maxResults = 0, long skip = 0, const std::vector<SortColumn> &sortColumns = {});

        /**
         * @brief Removes old lambda logs
         *
         * @param cutOff cut off time point
         * @return numberof logs removed
         */
        long RemoveExpiredLambdaLogs(const system_clock::time_point &cutOff);

        /**
         * @brief Deletes a lambda result counter
         *
         * @param oid lambda function oid
         * @return number of results deleted
         */
        [[nodiscard]] long DeleteResultsCounter(const std::string &oid);

        /**
         * @brief Deletes all lambda result counter for a lambda function
         *
         * @param lambdaArn lambda function ARN
         * @return number of results deleted
         */
        long DeleteResultsCounters(const std::string &lambdaArn);

        /**
         * @brief Deletes all lambda result counters
         *
         * @return number of results deleted
         */
        long DeleteAllResultsCounters();

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
         * @return number of lambda entities deleted
         * @throws DatabaseException
         */
        long DeleteAllLambdas();

      private:

        /**
         * Lambda map
         */
        std::map<std::string, Entity::Lambda::Lambda> _lambdas{};

        /**
         * Lambda result map
         */
        std::map<std::string, Entity::Lambda::LambdaResult> _lambdaResults{};

        /**
         * Lambda mutex
         */
        static boost::mutex _lambdaMutex;

        /**
         * Lambda result mutex
         */
        static boost::mutex _lambdaResultMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_LAMBDA_MEMORYDB_H
