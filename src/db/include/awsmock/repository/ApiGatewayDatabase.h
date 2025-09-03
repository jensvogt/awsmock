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

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * APIO gateway collection name
         */
        std::string _apiGatewayCollectionName;

        /**
         * Application in-memory database
         */
        ApiGatewayMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_API_GATEWAY_DATABASE_H
