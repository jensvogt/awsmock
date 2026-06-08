//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <ranges>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/Linq.h>
#include <awsmock/core/PagingUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/lambda/Lambda.h>
#include <awsmock/entity/lambda/LambdaResult.h>
#include <awsmock/repository/lambda/ILambdaRepository.h>

namespace Awsmock::Database {

  /**
   * Lambda in-memory database.
   *
   * @author jens.vogt\@opitz-consulting.com
   */
  class LambdaMemoryRepository final : public ILambdaRepository {

  public:
    /**
     * @brief Constructor
     */
    LambdaMemoryRepository() = default;

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
    bool lambdaExists(const std::string &region, const std::string &function, const std::string &runtime) const override;

    /**
     * @brief Check the existence of lambda
     *
     * @param lambda AWS function
     * @return true if lambda already exists
     * @throws DatabaseException
     */
    [[nodiscard]]
    bool lambdaExists(const Entity::Lambda::Lambda &lambda) const override;

    /**
     * @brief Check the existence of lambda
     *
     * @param function AWS function
     * @return true if lambda already exists
     * @throws DatabaseException
     */
    [[nodiscard]]
    bool lambdaExists(const std::string &function) const override;

    /**
     * Check the existence of lambda
     *
     * @param arn AWS ARN
     * @return true if lambda exists
     * @throws DatabaseException
     */
    [[nodiscard]]
    bool lambdaExistsByArn(const std::string &arn) const override;

    /**
     * @brief Create a new lambda function
     *
     * @param lambda lambda entity
     * @return created lambda entity.
     */
    [[nodiscard]]
    Entity::Lambda::Lambda createLambda(Entity::Lambda::Lambda &lambda) const override;

    /**
     * @brief Create a new lambda function or update an existing
     *
     * @param lambda lambda entity
     * @return created lambda entity.
     */
    [[nodiscard]]
    Entity::Lambda::Lambda createOrUpdateLambda(Entity::Lambda::Lambda &lambda) const override;

    /**
     * @brief Returns a lambda entity by primary key
     *
     * @param oid lambda primary key
     * @return lambda entity
     * @throws DatabaseException
     */
    [[nodiscard]]
    Entity::Lambda::Lambda getLambdaById(const std::string &oid) const override;

    /**
     * @brief Returns a lambda entity by primary key
     *
     * @param oid lambda primary key
     * @return lambda entity
     * @throws DatabaseException
     */
    [[nodiscard]]
    Entity::Lambda::Lambda getLambdaById(const bsoncxx::oid &oid) const override;

    /**
      * Returns a lambda entity by ARN
      *
      * @param arn lambda ARN
      * @return lambda entity
      * @throws DatabaseException
      */
    [[nodiscard]]
    Entity::Lambda::Lambda getLambdaByArn(const std::string &arn) const override;

    /**
     * @brief Returns a lambda entity by name
     *
     * @param region AWS region
     * @param name function name
     * @return lambda entity
     * @throws DatabaseException
     */
    [[nodiscard]]
    Entity::Lambda::Lambda getLambdaByName(const std::string &region, const std::string &name) const override;

    /**
     * Count all lambdas
     *
     * @param region aws-mock region.
     * @return total number of lambdas.
     */
    [[nodiscard]]
    long lambdaCount(const std::string &region) const override;

    /**
     * @brief Returns a list of lambda functions.
     *
     * @param region AWS region name
     * @return list of lambda functions
     */
    [[nodiscard]]
    std::vector<Entity::Lambda::Lambda> listLambdas(const std::string &region) const override;

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
    std::vector<Entity::Lambda::Lambda> listLambdaCounters(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

    /**
     * @brief Returns a list of lambda functions with the given event source ARN attached.
     *
     * @param eventSourceArn event source ARN
     * @return list of lambda functions
     */
    [[nodiscard]]
    std::vector<Entity::Lambda::Lambda> listLambdasWithEventSource(const std::string &eventSourceArn) const override;

    /**
     * @brief Updates an existing lambda function
     *
     * @param lambda lambda entity
     * @return updated lambda entity.
     */
    [[nodiscard]]
    Entity::Lambda::Lambda updateLambda(Entity::Lambda::Lambda &lambda) const override;

    /**
     * @brief Sets the status of a lambda instance
     *
     * @param containerId lambda container ID
     * @param status lambda instance status
     * @throws DatabaseException
     */
    void setInstanceValues(const std::string &containerId, const Entity::Lambda::LambdaInstanceStatus &status) const override;

    /**
     * @brief Sets the status of a lambda instance
     *
     * @param lambda lambda function
     * @param invocations number of invocations
     * @param avgRuntime average runtime
     * @throws DatabaseException
     */
    void setLambdaValues(const Entity::Lambda::Lambda &lambda, long invocations, long avgRuntime) const override;

    /**
     * @brief Import a lambda function
     *
     * @param lambda lambda entity
     * @return imported lambda entity.
     */
    [[nodiscard]]
    Entity::Lambda::Lambda importLambda(Entity::Lambda::Lambda &lambda) const override;

    /**
     * @brief Export a list of lambdas
     *
     * @param sortColumns sorting columns
     * @return Ã¶list of lambda entries
     */
    [[nodiscard]]
    std::vector<Entity::Lambda::Lambda> exportLambdas(const std::vector<SortColumn> &sortColumns) const override;

    /**
     * @brief Creates a new lambda result
     *
     * @param lambdaResult lambda result record
     * @return created lambdaResult entity
     */
    [[nodiscard]]
    Entity::Lambda::LambdaResult createLambdaResult(Entity::Lambda::LambdaResult &lambdaResult) const override;

    /**
     * @brief Returns the existence of a lambda result
     *
     * @param oid lambda result record OID
     * @return true if the result with the given OID exists
     */
    [[nodiscard]]
    bool lambdaResultExists(const std::string &oid) const override;

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
    [[nodiscard]]
    std::vector<Entity::Lambda::LambdaResult> listLambdaResultCounters(const std::string &lambdaArn, const std::string &prefix, long maxResults, long skip, const std::vector<SortColumn> &sortColumns) const override;

    /**
     * @brief Returns the total number of lambda results
     *
     * @param lambdaArn lambda function ARN
     * @return total number of results
     */
    [[nodiscard]]
    long lambdaResultsCount(const std::string &lambdaArn) const override;

    /**
     * @brief Returns a lambda function result.
     *
     * @param oid lambda result OID
     * @return lambda function result counter
     */
    [[nodiscard]]
    Entity::Lambda::LambdaResult getLambdaResultCounter(const std::string &oid) const override;

    /**
     * @brief Removes old lambda logs
     *
     * @param cutOff cut off time point
     * @return number of logs removed
     */
    [[nodiscard]]
    long removeExpiredLambdaLogs(const system_clock::time_point &cutOff) const override;

    /**
     * @brief Deletes a lambda result counter
     *
     * @param oid lambda function oid
     * @return number of results deleted
     */
    [[nodiscard]]
    long deleteResultsCounter(const std::string &oid) const override;

    /**
     * @brief Deletes all lambda result counter for a lambda function
     *
     * @param lambdaArn lambda function ARN
     * @return number of results deleted
     */
    [[nodiscard]]
    long deleteResultsCounters(const std::string &lambdaArn) const override;

    /**
     * @brief Deletes all lambda result counters
     *
     * @return number of results deleted
     */
    [[nodiscard]]
    long deleteAllResultsCounters() const override;

    /**
     * @brief Deletes an existing lambda function
     *
     * @param functionName lambda function name
     * @throws DatabaseException
     */
    void deleteLambda(const std::string &functionName) const override;

    /**
     * @brief Deletes all existing lambda functions
     *
     * @return number of lambda entities deleted
     * @throws DatabaseException
     */
    [[nodiscard]]
    long deleteAllLambdas() const override;

  private:
    /**
     * @brief Channeled logger
     */
    mutable logger_t _logger{boost::log::keywords::channel = "Lambda"};

    /**
     * Lambda map
     */
    mutable std::unordered_map<std::string, Entity::Lambda::Lambda> _lambdas{};

    /**
     * Lambda result map
     */
    mutable std::unordered_map<std::string, Entity::Lambda::LambdaResult> _lambdaResults{};

    /**
     * Lambda mutex
     */
    static boost::mutex _lambdaMutex;

    /**
     * Lambda result mutex
     */
    static boost::mutex _lambdaResultMutex;
  };

} // namespace Awsmock::Database
