//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <map>
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/Linq.h>
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/entity/apigateway/RestApi.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/apigateway/IApiGatewayRepository.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief API gateway in-memory database.
     *
     * @par
     * Provides an in-memory database using a simple hash map. The key is a randomly generated UUID.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayMemoryRepository final : public IApiGatewayRepository {

      public:

        /**
         * @brief Constructor
         */
        ApiGatewayMemoryRepository() = default;

        /**
         * @brief Check the existence of an API key
         *
         * @param id API key ID
         */
        [[nodiscard]] bool apiKeyExists(const std::string &id) const override;

        /**
         * @brief Check the existence of an API key
         *
         * @param region AWS region
         * @param name API key name
         */
        [[nodiscard]] bool apiKeyExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Create a new API gateway key
         *
         * @param key API gateway key to create
         * @return created api key
         */
        Entity::ApiGateway::ApiKey createKey(Entity::ApiGateway::ApiKey &key) const override;

        /**
         * @brief Returns a list of API keys
         *
         * @param nameQuery name query
         * @param customerId customer ID
         * @param position current position
         * @param limit maximal number of keys
         * @return created api key
         */
        std::vector<Entity::ApiGateway::ApiKey> getApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, long limit) const override;

        /**
         * @brief Returns an API key by ID
         *
         * @param id name query
         * @return api key
         */
        [[nodiscard]] Entity::ApiGateway::ApiKey getApiKeyById(const std::string &id) const override;

        /**
         * @brief Updates an existing API key
         *
         * @param key API key to update
         * @return updated api key
         */
        [[nodiscard]] Entity::ApiGateway::ApiKey updateApiKey(Entity::ApiGateway::ApiKey &key) const override;

        /**
         * @brief Import an API key
         *
         * @par
         * If the provided API key exists already, it will be updated, otherwise inserted. The modified/created timestamp will be updated accordingly.
         *
         * @param key API key to import
         */
        void importApiKey(Entity::ApiGateway::ApiKey &key) const override;

        /**
         * @brief Returns the total number of keys
         *
         * @return API key count
         */
        [[nodiscard]] long countApiKeys() const override;

        /**
         * @brief Delete an API gateway key by ID
         *
         * @param id API gateway key ID
         */
        void deleteKey(const std::string &id) const override;

        /**
         * @brief Delete all keys
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        long deleteAllKeys() const override;

        /**
         * @brief Check the existence of an REST API
         *
         * @param id REST API ID
         */
        [[nodiscard]] bool restApiExists(const std::string &id) const override;

        /**
         * @brief Check the existence of an REST API
         *
         * @param region AWS region
         * @param name REST API name
         */
        [[nodiscard]] bool restApiExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Create a new REST API
         *
         * @param restApi REST API entity to create
         * @return created REST API entity
         */
        [[nodiscard]] Entity::ApiGateway::RestApi createRestApi(Entity::ApiGateway::RestApi &restApi) const override;

        /**
         * @brief Create a new REST API or updates an existing one.
         *
         * @param restApi REST API entity to create or update
         * @return created or updated REST API entity
         */
        [[nodiscard]]
        Entity::ApiGateway::RestApi upsertRestApi(Entity::ApiGateway::RestApi &restApi) const override;

        /**
         * @brief Returns a list of API key counters
         *
         * @param prefix name prefix
         * @param pageSize page size customer ID
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of API key counters
         */
        [[nodiscard]] std::vector<Entity::ApiGateway::ApiKey> listApiKeyCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns a list of REST API counters
         *
         * @param prefix name prefix
         * @param pageSize page size customer ID
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of REST API counters
         */
        [[nodiscard]] std::vector<Entity::ApiGateway::RestApi> listRestApiCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

        /**
         * API key map
         */
        mutable std::unordered_map<std::string, Entity::ApiGateway::ApiKey> _apiKeys{};

        /**
         * REST API map
         */
        mutable std::unordered_map<std::string, Entity::ApiGateway::RestApi> _restApis{};

        /**
         * API key mutex
         */
        static boost::mutex _apiKeyMutex;

        /**
         * REST API mutex
         */
        static boost::mutex _restApiMutex;
    };

}// namespace Awsmock::Database
