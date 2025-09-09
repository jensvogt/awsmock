//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_API_GATEWAY_MEMORYDB_H
#define AWSMOCK_REPOSITORY_API_GATEWAY_MEMORYDB_H

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
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    /**
     * @brief API gateway in-memory database.
     *
     * @par
     * Provides an in-memory database using a simple hash map. The key is a randomly generated UUID.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        ApiGatewayMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static ApiGatewayMemoryDb &instance() {
            static ApiGatewayMemoryDb apiGatewayMemoryDb;
            return apiGatewayMemoryDb;
        }

        /**
         * @brief Check the existence of an API key
         *
         * @param id API key ID
         */
        [[nodiscard]] bool ApiKeyExists(const std::string &id) const;

        /**
         * @brief Check the existence of an API key
         *
         * @param region AWS region
         * @param name API key name
         */
        [[nodiscard]] bool ApiKeyExists(const std::string &region, const std::string &name) const;

        /**
         * @brief Create a new API gateway key
         *
         * @param key API gateway key to create
         * @return created api key
         */
        Entity::ApiGateway::ApiKey CreateKey(const Entity::ApiGateway::ApiKey &key);

        /**
         * @brief Returns a list of API keys
         *
         * @param nameQuery name query
         * @param customerId customer ID
         * @param position current position
         * @param limit maximal number of keys
         * @return created api key
         */
        std::vector<Entity::ApiGateway::ApiKey> GetApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, long limit);

        /**
         * @brief Returns an API key by ID
         *
         * @param id name query
         * @return api key
         */
        [[nodiscard]] Entity::ApiGateway::ApiKey GetApiKeyById(const std::string &id);

        /**
         * @brief Updates an existing API key
         *
         * @param key API key to update
         * @return updated api key
         */
        [[nodiscard]] Entity::ApiGateway::ApiKey UpdateApiKey(const Entity::ApiGateway::ApiKey &key);

        /**
         * @brief Import an API key
         *
         * @par
         * If the provided API key exists already, it will be updated, otherwise inserted. The modified/created timestamp will be updated accordingly.
         *
         * @param key API key to import
         */
        void ImportApiKey(Entity::ApiGateway::ApiKey &key);

        /**
         * @brief Returns the total number of keys
         *
         * @return API key count
         */
        [[nodiscard]] long CountApiKeys() const;

        /**
         * @brief Delete an API gateway key by ID
         *
         * @param id API gateway key ID
         */
        void DeleteKey(const std::string &id);

        /**
         * @brief Delete all keys
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        long DeleteAllKeys();

        /**
         * @brief Check the existence of an REST API
         *
         * @param id REST API ID
         */
        [[nodiscard]] bool RestApiExists(const std::string &id) const;

        /**
         * @brief Check the existence of an REST API
         *
         * @param region AWS region
         * @param name REST API name
         */
        [[nodiscard]] bool RestApiExists(const std::string &region, const std::string &name) const;

        /**
         * @brief Create a new REST API
         *
         * @param restApi REST API entity to create
         * @return created REST API entity
         */
        [[nodiscard]] Entity::ApiGateway::RestApi CreateRestApi(const Entity::ApiGateway::RestApi &restApi);

        /**
         * @brief Returns a list of API key counters
         *
         * @param prefix name prefix
         * @param pageSize page size customer ID
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of API key counters
         */
        [[nodiscard]] std::vector<Entity::ApiGateway::ApiKey> ListApiKeyCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const;

        /**
         * @brief Returns a list of REST API counters
         *
         * @param prefix name prefix
         * @param pageSize page size customer ID
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of REST API counters
         */
        [[nodiscard]] std::vector<Entity::ApiGateway::RestApi> ListRestApiCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const;

      private:

        /**
         * API key map
         */
        std::map<std::string, Entity::ApiGateway::ApiKey> _apiKeys{};

        /**
         * REST API map
         */
        std::map<std::string, Entity::ApiGateway::RestApi> _restApis{};

        /**
         * API key mutex
         */
        static boost::mutex _apiKeyMutex;

        /**
         * REST API mutex
         */
        static boost::mutex _restApiMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_API_GATEWAY_MEMORYDB_H
