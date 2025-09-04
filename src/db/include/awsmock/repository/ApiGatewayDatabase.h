//
// Created by vogje01 on 29/11/2023.
//

#ifndef AWSMOCK_REPOSITORY_API_GATEWAY_DATABASE_H
#define AWSMOCK_REPOSITORY_API_GATEWAY_DATABASE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/memorydb/ApiGatewayMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief API gateway MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ApiGatewayDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit ApiGatewayDatabase();

        /**
         * @brief Singleton instance
         */
        static ApiGatewayDatabase &instance() {
            static ApiGatewayDatabase apiGatewayDatabase;
            return apiGatewayDatabase;
        }

        /**
         * @brief Initialize the database
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
        Entity::ApiGateway::Key CreateKey(Entity::ApiGateway::Key &key) const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * API gateway key collection name
         */
        std::string _apiKeyCollectionName;

        /**
         * Application in-memory database
         */
        ApiGatewayMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_API_GATEWAY_DATABASE_H
