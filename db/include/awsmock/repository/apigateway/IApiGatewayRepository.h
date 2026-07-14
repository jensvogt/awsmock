//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/entity/apigateway/RestApi.h>
#include <awsmock/entity/apigateway/UsagePlan.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for application repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * Cognito-related data.
     */
    class IApiGatewayRepository {

      public:

        /**
         * @brief Virtual destructor for the IApplicationRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~IApiGatewayRepository() = default;

        /**
         * @brief Check the existence of an API key
         *
         * @param id API key ID
         */
        [[nodiscard]]
        virtual bool apiKeyExists(const std::string &id) const = 0;

        /**
         * @brief Check the existence of an API key
         *
         * @param region AWS region
         * @param name API key name
         */
        [[nodiscard]]
        virtual bool apiKeyExists(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Create a new API gateway key
         *
         * @param key API gateway key to create
         * @return created api key
         */
        [[nodiscard]]
        virtual Entity::ApiGateway::ApiKey createKey(Entity::ApiGateway::ApiKey &key) const = 0;

        /**
         * @brief Returns a list of API keys
         *
         * @param nameQuery name query
         * @param customerId customer ID
         * @param position current position
         * @param limit maximal number of keys
         * @return list of api keys
         */
        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::ApiKey> listApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, long limit) const = 0;

        /**
         * @brief Returns a list of all API keys
         *
         * @return list of api keys
         */
        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::ApiKey> listApiKeys() const = 0;

        /**
         * @brief Returns an API key by ID
         *
         * @param id name query
         * @return api key
         */
        [[nodiscard]]
        virtual Entity::ApiGateway::ApiKey getApiKeyById(const std::string &id) const = 0;

        /**
         * @brief Updates an existing API key
         *
         * @param key API key to update
         * @return updated api key
         */
        [[nodiscard]]
        virtual Entity::ApiGateway::ApiKey updateApiKey(Entity::ApiGateway::ApiKey &key) const = 0;

        /**
         * @brief Import an API key
         *
         * @par
         * If the provided API key exists already, it will be updated, otherwise inserted. The modified/created timestamp will be updated accordingly.
         *
         * @param key API key to import
         */
        virtual void importApiKey(Entity::ApiGateway::ApiKey &key) const = 0;

        /**
         * @brief Import REST APIs.
         *
         * @par
         * If the provided REST API exists already, it will be updated, otherwise inserted. The modified/created timestamp will be updated accordingly.
         *
         * @param restApi REST API to import
         */
        virtual void importRestApis(Entity::ApiGateway::RestApi &restApi) const = 0;

        /**
         * @brief Import API gateway usage plans
         *
         * @par
         * If the provided usage plan exists already, it will be updated, otherwise inserted. The modified/created timestamp will be updated accordingly.
         *
         * @param usagePlan API gateway usage plan
         */
        virtual void importUsagePlan(Entity::ApiGateway::UsagePlan &usagePlan) const = 0;

        /**
         * @brief Returns the total number of keys
         *
         * @return API key count
         */
        [[nodiscard]]
        virtual long countApiKeys() const = 0;

        /**
         * @brief Delete an API gateway key by ID
         *
         * @param id API gateway key ID
         */
        virtual void deleteKey(const std::string &id) const = 0;

        /**
         * @brief Delete all keys
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllKeys() const = 0;

        /**
         * @brief Check the existence of an REST API
         *
         * @param id REST API ID
         */
        [[nodiscard]]
        virtual bool restApiExists(const std::string &id) const = 0;

        /**
         * @brief Check the existence of an REST API
         *
         * @param restApiId REST API ID
         */
        [[nodiscard]]
        virtual bool restApiExistsByRestApiId(const std::string &restApiId) const = 0;

        /**
         * @brief Check the existence of an REST API
         *
         * @param region AWS region
         * @param name REST API name
         */
        [[nodiscard]]
        virtual bool restApiExists(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Create a new REST API
         *
         * @param restApi REST API entity to create
         * @return created REST API entity
         */
        [[nodiscard]]
        virtual Entity::ApiGateway::RestApi createRestApi(Entity::ApiGateway::RestApi &restApi) const = 0;

        /**
         * @brief Create a new REST API or updates an existing one.
         *
         * @param restApi REST API entity to create or update
         * @return created or updated REST API entity
         */
        [[nodiscard]]
        virtual Entity::ApiGateway::RestApi upsertRestApi(Entity::ApiGateway::RestApi &restApi) const = 0;

        /**
         * @brief Returns a list of API key counters
         *
         * @param prefix name prefix
         * @param pageSize page size customer ID
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of API key counters
         */
        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::ApiKey> listApiKeyCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns a list of REST API counters
         *
         * @param prefix name prefix
         * @param pageSize page size customer ID
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of REST API counters
         */
        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::RestApi> listRestApiCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns a list of REST API counters
         *
         * @param region AWS region
         * @return list of REST API counters
         */
        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::RestApi> listRestApis(const std::string &region) const = 0;

        /**
         * @brief Returns a list of all REST API counters
         *
         * @return list of REST API counters
         */
        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::RestApi> listRestApis() const = 0;

        /**
         * @brief Returns the total number of REST Apis
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return REST API count
         */
        [[nodiscard]]
        virtual long countRestApis(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Returns a REST API entity
         *
         * @param region AWS region
         * @param prefix name
         * @return REST API entity
         */
        [[nodiscard]]
        virtual Entity::ApiGateway::RestApi getRestApi(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Returns a REST API entity
         *
         * @param restApiId rest API ID
         * @return REST API entity
         */
        [[nodiscard]]
        virtual Entity::ApiGateway::RestApi getRestApi(const std::string &restApiId) const = 0;

        /**
         * @brief Deletes a REST API entity
         *
         * @param region AWS region
         * @param name APi name
         * @return number of REST APIs deleted
         */
        [[nodiscard]]
        virtual long deleteRestApi(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Deletes a REST API entity
         *
         * @param restApiId REST API ID AWS region
         * @return number of REST APIs deleted
         */
        [[nodiscard]]
        virtual long deleteRestApi(const std::string &restApiId) const = 0;

        [[nodiscard]]
        virtual bool usagePlanExists(const std::string &id) const = 0;

        [[nodiscard]]
        virtual Entity::ApiGateway::UsagePlan createUsagePlan(Entity::ApiGateway::UsagePlan &usagePlan) const = 0;

        [[nodiscard]]
        virtual Entity::ApiGateway::UsagePlan upsertUsagePlan(Entity::ApiGateway::UsagePlan &usagePlan) const = 0;

        [[nodiscard]]
        virtual Entity::ApiGateway::UsagePlan getUsagePlanById(const std::string &id) const = 0;

        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::UsagePlan> listUsagePlans() const = 0;

        [[nodiscard]]
        virtual std::vector<Entity::ApiGateway::UsagePlan> listUsagePlanCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        [[nodiscard]]
        virtual long countUsagePlans(const std::string &region, const std::string &prefix) const = 0;

        virtual void deleteUsagePlan(const std::string &id) const = 0;

        [[nodiscard]]
        virtual long deleteAllUsagePlans() const = 0;
    };

}// namespace Awsmock::Database
