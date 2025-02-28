//
// Created by vogje01 on 07/05/2024.
//

#ifndef AWSMOCK_CORE_CONNECTION_POOL_H
#define AWSMOCK_CORE_CONNECTION_POOL_H

// C++ includes
#include <memory>
#include <ranges>

// MongoDB includes
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Database {

    /**
     * @brief MongoDB connection pool.
     *
     * Singleton connection pool.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class ConnectionPool {

      public:

        /**
         * @brief Singleton instance
         *
         * @return singleton
         */
        static ConnectionPool &instance() {
            static ConnectionPool connectionPool;
            return connectionPool;
        }

        /**
         * @brief Configure the connection pool
         */
        void Configure();

        /**
         * @brief Return a connection from the pool
         *
         * @return connection from the pool
         */
        [[nodiscard]] mongocxx::pool::entry GetConnection() const;

        /**
         * @brief Try to get a connection
         *
         * @return optional connection from the pool
         */
        [[nodiscard]] bsoncxx::stdx::optional<mongocxx::pool::entry> TryGetConnection() const;

        /**
         * @brief Shutdown the connection pool
         */
        void Shutdown();

        /**
         * @brief Private constructor
         */
        ConnectionPool() = default;

      private:

        /**
         * MongoDB instance
         */
        std::unique_ptr<mongocxx::instance> _instance = nullptr;

        /**
         * Connection pool
         */
        std::unique_ptr<mongocxx::pool> _pool = nullptr;
    };

}// namespace AwsMock::Database

#endif//AWSMOCK_CORE_CONNECTION_POOL_H
