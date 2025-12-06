//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_DATABASE_H
#define AWSMOCK_REPOSITORY_DATABASE_H

// C++ standard includes
#include <string>

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
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/utils/ConnectionPool.h>

namespace AwsMock::Database {

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
    };

    /**
     * @brief MongoDB database base class.
     *
     * @par
     * Indexes are created during startup using detached background threads. Indexes which are already existing are ignored.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit DatabaseBase();

        /**
         * @brief Returns a MongoDB connection from the pool
         *
         * @return MongoDB database client
         */
        [[nodiscard]] mongocxx::database GetConnection() const;

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
        std::string _name;

        /**
         * Database client
         */
        std::unique_ptr<mongocxx::pool> _pool{};

        /**
         * Database flag
         */
        bool _useDatabase;

        /**
         * @brief Index definitions
         */
        const static std::map<std::string, IndexDefinition> indexDefinitions;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_DATABASE_H
