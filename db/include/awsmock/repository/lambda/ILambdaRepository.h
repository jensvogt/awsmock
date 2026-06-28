//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/entity/lambda/LambdaResult.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for SQS repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * SQS-related data.
     */
    class ILambdaRepository {

      public:

        /**
         * @brief Virtual destructor for the ISQSRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~ILambdaRepository() = default;

        /**
         * @brief Check the existence of lambda
         *
         * @param region AWS region name
         * @param function AWS function
         * @param runtime lambda runtime
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool lambdaExists(const std::string &region, const std::string &function, const std::string &runtime) const = 0;

        /**
         * Check the existence of lambda
         *
         * @param lambda AWS function
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool lambdaExists(const Entity::Lambda::Lambda &lambda) const = 0;

        /**
         * @brief Check the existence of lambda
         *
         * @param functionName AWS function
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool lambdaExists(const std::string &functionName) const = 0;

        /**
         * @brief Check the existence of lambda
         *
         * @param arn AWS ARN
         * @return true if lambda exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool lambdaExistsByArn(const std::string &arn) const = 0;

        /**
         * @brief Create a new lambda function
         *
         * @param lambda lambda entity
         * @return created lambda entity.
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda createLambda(Entity::Lambda::Lambda &lambda) const = 0;

        /**
         * @brief Count all lambdas
         *
         * @param region aws-mock region.
         * @return total number of lambdas.
         */
        [[nodiscard]]
        virtual long lambdaCount(const std::string &region) const = 0;

        /**
         * @brief Updates an existing lambda function
         *
         * @param lambda lambda entity
         * @return updated lambda entity.
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda updateLambda(Entity::Lambda::Lambda &lambda) const = 0;

        /**
         * @brief Created or updates an existing lambda function
         *
         * @param lambda lambda entity
         * @return created or updated lambda entity.
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda createOrUpdateLambda(Entity::Lambda::Lambda &lambda) const = 0;

        /**
         * @brief Import a lambda function
         *
         * @param lambda lambda entity
         * @return imported lambda entity.
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda importLambda(Entity::Lambda::Lambda &lambda) const = 0;

        /**
         * @brief Returns a lambda entity by primary key
         *
         * @param oid lambda primary key
         * @return lambda entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda getLambdaById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns a lambda entity by primary key
         *
         * @param oid lambda primary key
         * @return lambda entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda getLambdaById(const std::string &oid) const = 0;

        /**
         * @brief Returns a lambda entity by ARN
         *
         * @param arn lambda ARN
         * @return lambda entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda getLambdaByArn(const std::string &arn) const = 0;

        /**
         * @brief Returns a lambda entity by name
         *
         * @param region AWS region
         * @param name function name
         * @return lambda entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Lambda::Lambda getLambdaByName(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Returns a list of lambda functions.
         *
         * @param region AWS region name
         * @return list of lambda functions
         */
        [[nodiscard]]
        virtual std::vector<Entity::Lambda::Lambda> listLambdas(const std::string &region) const = 0;

        /**
         * @brief Returns a list of lambda functions.
         *
         * @param region AWS region
         * @param prefix name prefix
         * @param pageSize maximal number of results
         * @param pageIndex number of records to skip
         * @param sortColumns sorting columns
         * @return list of lambda function counters
         */
        [[nodiscard]]
        virtual std::vector<Entity::Lambda::Lambda> listLambdaCounters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Export a list of lambdas
         *
         * @param sortColumns sorting columns
         * @return Ã¶list of lambda entries
         */
        [[nodiscard]]
        virtual std::vector<Entity::Lambda::Lambda> exportLambdas(const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns a list of lambda functions with the given event source ARN attached.
         *
         * @param eventSourceArn event source ARN
         * @return list of lambda functions
         */
        [[nodiscard]]
        virtual std::vector<Entity::Lambda::Lambda> listLambdasWithEventSource(const std::string &eventSourceArn) const = 0;

        /**
         * @brief Creates a new lambda result
         *
         * @param lambdaResult lambda result record
         * @return created lambdaResult entity
         */
        [[nodiscard]]
        virtual Entity::Lambda::LambdaResult createLambdaResult(Entity::Lambda::LambdaResult &lambdaResult) const = 0;

        /**
         * @brief Returns the existence of a lambda result
         *
         * @param oid lambda result record OID
         * @return true if the result with the given OID exists
         */
        [[nodiscard]]
        virtual bool lambdaResultExists(const std::string &oid) const = 0;

        /**
         * @brief Removes old lambda logs
         *
         * @param cutOff cut off time point
         * @return number of logs removed
         */
        [[nodiscard]]
        virtual long removeExpiredLambdaLogs(const system_clock::time_point &cutOff) const = 0;

        /**
         * @brief Returns a lambda function result.
         *
         * @param oid lambda result OID
         * @return lambda function result counter
         */
        [[nodiscard]]
        virtual Entity::Lambda::LambdaResult getLambdaResultCounter(const std::string &oid) const = 0;

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
        [[nodiscard]]
        virtual std::vector<Entity::Lambda::LambdaResult> listLambdaResultCounters(const std::string &lambdaArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns the total number of lambda results
         *
         * @param lambdaArn lambda function ARN
         * @return total number of results
         */
        [[nodiscard]]
        virtual long lambdaResultsCount(const std::string &lambdaArn) const = 0;

        /**
         * @brief Deletes a lambda result counter
         *
         * @param oid lambda function oid
         * @return number of results deleted
         */
        [[nodiscard]]
        virtual long deleteResultsCounter(const std::string &oid) const = 0;

        /**
         * @brief Deletes all lambda result counter for a lambda function
         *
         * @param lambdaArn lambda function ARN
         * @return number of results deleted
         */
        [[nodiscard]]
        virtual long deleteResultsCounters(const std::string &lambdaArn) const = 0;

        /**
         * @brief Deletes all lambda result counters
         *
         * @return number of results deleted
         */
        [[nodiscard]]
        virtual long deleteAllResultsCounters() const = 0;

        /**
         * @brief Deletes an existing lambda function
         *
         * @param functionName lambda function name
         * @throws DatabaseException
         */
        virtual void deleteLambda(const std::string &functionName) const = 0;

        /**
         * @brief Deletes all existing lambda functions
         *
         * @return number of lambda objects deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllLambdas() const = 0;
    };

}// namespace Awsmock::Database