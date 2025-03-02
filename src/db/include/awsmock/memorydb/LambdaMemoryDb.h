//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_LAMBDA_MEMORYDB_H
#define AWSMOCK_REPOSITORY_LAMBDA_MEMORYDB_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/lambda/Lambda.h>
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
         * Constructor
         */
        LambdaMemoryDb() = default;

        /**
         * Singleton instance
         */
        static LambdaMemoryDb &instance() {
            static LambdaMemoryDb lambdaMemoryDb;
            return lambdaMemoryDb;
        }

        /**
         * Check existence of lambda
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
         * Check existence of lambda
         *
         * @param function AWS function
         * @return true if lambda already exists
         * @throws DatabaseException
         */
        bool LambdaExists(const std::string &function);

        /**
         * Check existence of lambda
         *
         * @param arn AWS ARN
         * @return true if lambda exists
         * @throws DatabaseException
         */
        bool LambdaExistsByArn(const std::string &arn);

        /**
         * Create a new lambda function
         *
         * @param lambda lambda entity
         * @return created lambda entity.
         */
        Entity::Lambda::Lambda CreateLambda(const Entity::Lambda::Lambda &lambda);

        /**
         * Returns a lambda entity by primary key
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
         * Returns a lambda entity by name
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
         * Returns a list of lambda functions.
         *
         * @param region AWS region name
         * @return list of lambda functions
         */
        std::vector<Entity::Lambda::Lambda> ListLambdas(const std::string &region);

        /**
         * @brief Returns a list of lambda functions with the given event source ARN attached.
         *
         * @param eventSourceArn event source ARN
         * @return list of lambda functions
         */
        std::vector<Entity::Lambda::Lambda> ListLambdasWithEventSource(const std::string &eventSourceArn);

        /**
         * Updates an existing lambda lambda function
         *
         * @param lambda lambda entity
         * @return updated lambda entity.
         */
        Entity::Lambda::Lambda UpdateLambda(const Entity::Lambda::Lambda &lambda);

        /**
         * @brief Sets the status of an lambda instance
         *
         * @param containerId lambda container ID
         * @param status lambda instance status
         * @throws DatabaseException
         */
        void SetInstanceStatus(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status);

        /**
         * Deletes an existing lambda function
         *
         * @param functionName lambda function name
         * @throws DatabaseException
         */
        void DeleteLambda(const std::string &functionName);

        /**
         * Deletes all existing lambda functions
         *
         * @throws DatabaseException
         */
        void DeleteAllLambdas();

      private:

        /**
         * Lambda map
         */
        std::map<std::string, Entity::Lambda::Lambda> _lambdas{};

        /**
         * Lambda mutex
         */
        static boost::mutex _lambdaMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_LAMBDA_MEMORYDB_H
