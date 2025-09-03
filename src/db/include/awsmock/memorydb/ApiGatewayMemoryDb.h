//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_API_GATEWAY_MEMORYDB_H
#define AWSMOCK_REPOSITORY_API_GATEWAY_MEMORYDB_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/apps/Application.h>
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

      private:

        /**
         * Application map
         */
        std::map<std::string, Entity::Apps::Application> _applications{};

        /**
         * API gateway mutex
         */
        static boost::mutex _apiGatewayMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_API_GATEWAY_MEMORYDB_H
