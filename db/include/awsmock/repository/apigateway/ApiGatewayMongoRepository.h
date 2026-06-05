//
// Created by vogje01 on 29/11/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/apigateway/ApiKey.h>
#include <awsmock/entity/apigateway/RestApi.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/DatabaseBase.h>
#include <awsmock/repository/apigateway/IApiGatewayRepository.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief API gateway MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayMongoRepository final : public IApiGatewayRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit ApiGatewayMongoRepository() = default;

        /**
         * @brief Check the existence of an API key
         *
         * @param id API key ID
         */
        [[nodiscard]]
        bool ApiKeyExists(const std::string &id) const override;

        /**
         * @brief Check the existence of an API key
         *
         * @param region AWS region
         * @param name API key name
         */
        [[nodiscard]]
        bool ApiKeyExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Create a new API gateway key
         *
         * @param key API gateway key to create
         * @return created api key
         */
        [[nodiscard]]
        Entity::ApiGateway::ApiKey CreateKey(Entity::ApiGateway::ApiKey &key) const override;

        /**
         * @brief Returns a list of API keys
         *
         * @param nameQuery name query
         * @param customerId customer ID
         * @param position current position
         * @param limit maximal number of keys
         * @return created api key
         */
        [[nodiscard]]
        std::vector<Entity::ApiGateway::ApiKey> GetApiKeys(const std::string &nameQuery, const std::string &customerId, const std::string &position, long limit) const override;

        /**
         * @brief Returns an API key by ID
         *
         * @param id name query
         * @return api key
         */
        [[nodiscard]]
        Entity::ApiGateway::ApiKey GetApiKeyById(const std::string &id) const override;

        /**
         * @brief Updates an existing API key
         *
         * @param key API key to update
         * @return updated api key
         */
        [[nodiscard]]
        Entity::ApiGateway::ApiKey UpdateApiKey(Entity::ApiGateway::ApiKey &key) const override;

        /**
         * @brief Import an API key
         *
         * @par
         * If the provided API key exists already, it will be updated, otherwise inserted. The modified/created timestamp will be updated accordingly.
         *
         * @param key API key to import
         */
        void ImportApiKey(Entity::ApiGateway::ApiKey &key) const override;

        /**
         * @brief Returns the total number of keys
         *
         * @return API key count
         */
        [[nodiscard]]
        long CountApiKeys() const override;

        /**
         * @brief Delete an API gateway key by ID
         *
         * @param id API gateway key ID
         */
        void DeleteKey(const std::string &id) const override;

        /**
         * @brief Delete all keys
         *
         * @return number of entities deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long DeleteAllKeys() const override;

        /**
         * @brief Check the existence of an REST API
         *
         * @param id REST API ID
         */
        [[nodiscard]]
        bool RestApiExists(const std::string &id) const override;

        /**
         * @brief Check the existence of an REST API
         *
         * @param region AWS region
         * @param name REST API name
         */
        [[nodiscard]]
        bool RestApiExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Create a new REST API
         *
         * @param restApi REST API entity to create
         * @return created REST API entity
         */
        [[nodiscard]]
        Entity::ApiGateway::RestApi CreateRestApi(Entity::ApiGateway::RestApi &restApi) const override;

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
        std::vector<Entity::ApiGateway::ApiKey> ListApiKeyCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

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
        std::vector<Entity::ApiGateway::RestApi> ListRestApiCounters(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "ApiGateway"};

        /**
         * @brief Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * @brief User pool collection name
         */
        static constexpr auto _apiKeyCollectionName = "apigateway_key";

        /**
         * @brief User pool collection name
         */
        static constexpr auto _restApiCollectionName = "apigateway_rest";
    };

}// namespace Awsmock::Database
