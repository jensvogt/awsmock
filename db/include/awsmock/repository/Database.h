//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// MongoDB includes
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/utils/ConnectionPool.h>

namespace Awsmock::Database {

    struct IndexColumnDefinition {

        /**
         * Column name
         */
        std::string columns;

        /**
         * Index direction
         */
        int direction;
    };

    struct IndexDefinition {

        /**
         * Collection name
         */
        std::string collectionName;

        /**
         * Columns definitions
         */
        std::vector<IndexColumnDefinition> indexColumns;

        /**
         * Unique index
         */
        bool unique = false;
    };

    /**
     * Represents a database connection and provides methods to interact with the database.
     *
     * The Database class is responsible for establishing a connection
     * to a database, executing queries, and managing transactional operations.
     */
    class Database {

      public:

        /**
         * Instance of the class
         */
        static Database &instance() {
            static Database instance;
            return instance;
        }

        /**
         * Represents a database entity.
         */
        Database(const Database &) = delete;

        /**
         * Overloaded operator for performing a specific operation.
         *
         * @param rhs The right-hand side operand to be used in the operation.
         * @return The result of the operation involving the current object and the right-hand side operand.
         */
        Database &operator=(const Database &rhs) = delete;

        /**
         * @brief Initialize the MongoDB connection pool
         */
        void initialize();

        /**
         * @brief Acquire a client from the pool
         */
        mongocxx::pool::entry client() const;

        /**
         * @brief Returns the database name
         */
        [[nodiscard]]
        const std::string &databaseName() const { return _databaseName; }

        /**
         * @brief Ping the server — throws if unreachable
         */
        void ping();

        /**
         * @brief Returns a MongoDB connection from the pool
         *
         * @return MongoDB database client
         */
        [[nodiscard]]
        mongocxx::database GetConnection() const;

        /**
         * @brief Check all indexes.
         *
         * <p>Normally done during manager StartServer.</p>
         */
        void CreateIndexes() const;

        /**
         * @brief Check whether we are running without database
         */
        [[nodiscard]] static bool HasDatabase();

        /**
         * @brief Returns the database name
         *
         * @return database name
         */
        [[nodiscard]] std::string GetDatabaseName() const;

      private:

        Database() = default;

        std::unique_ptr<mongocxx::pool> _pool{};
        std::string _databaseName;

        /**
         * @brief Create index as background thread
         *
         * @param database database connection
         * @param indexName name of the collection
         */
        static void CreateIndex(const mongocxx::database &database, const std::string &indexName);

        /**
         * Database name
         */
        std::string _name{};

        /**
         * Database flag
         */
        bool _useDatabase = true;

        /**
         * @brief Index definitions
         */
        const static std::map<std::string, IndexDefinition> indexDefinitions;
    };

}// namespace Awsmock::Database
