//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_DYNAMODB_MEMORYDB_H
#define AWSMOCK_REPOSITORY_DYNAMODB_MEMORYDB_H

// C++ standard includes
#include <string>
#include <vector>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Macros.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/dynamodb/Item.h>
#include <awsmock/entity/dynamodb/Table.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief DynamoDB in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        AWSMOCK_API DynamoDbMemoryDb() = default;

        /**
         * @brief Singleton instance
         *
         * @return singleton instance
         */
        static DynamoDbMemoryDb &instance() {
            static DynamoDbMemoryDb dynamoDbMemoryDb;
            return dynamoDbMemoryDb;
        }

        /**
         * @brief Check existence of DynamoDb table
         *
         * @param region AWS region name
         * @param tableName table name
         * @return true if DynamoDb table already exists
         * @throws DatabaseException
         */
        AWSMOCK_API bool TableExists(const std::string &region, const std::string &tableName);

        /**
         * @brief Create a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return created DynamoDb table.
         */
        AWSMOCK_API Entity::DynamoDb::Table CreateTable(const Entity::DynamoDb::Table &table);

        /**
         * @brief Updates a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return updated DynamoDb table.
         */
        AWSMOCK_API Entity::DynamoDb::Table UpdateTable(const Entity::DynamoDb::Table &table);

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Table GetTableById(const std::string &oid);

        /**
         * @brief Returns a table entity by region and name
         *
         * @param region table region
         * @param name table name
         * @return table entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Table GetTableByRegionName(const std::string &region, const std::string &name);

        /**
         * @brief Returns a list of DynamoDB tables
         *
         * @param region AWS region name
         * @return list of DynamoDB tables
         */
        AWSMOCK_API std::vector<Entity::DynamoDb::Table> ListTables(const std::string &region = {});

        /**
         * @brief Returns the number of DynamoDB tables
         *
         * @param region AWS region name
         * @return number of DynamoDB tables
         */
        AWSMOCK_API long CountTables(const std::string &region = {}) const;

        /**
         * @brief Deletes an existing DynamoDB table
         *
         * @param tableName name of the table
         * @throws DatabaseException
         */
        AWSMOCK_API void DeleteTable(const std::string &tableName);

        /**
         * @brief Deletes all existing DynamoDB tables
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        AWSMOCK_API long DeleteAllTables();

        /**
         * @brief Checks the existence of an item.
         *
         * @param item DynamoDB item
         * @return true if database exists, otherwise false
         * @throws DatabaseException
         */
        AWSMOCK_API bool ItemExists(const Entity::DynamoDb::Item &item);

        /**
         * @brief Returns a list of DynamoDB items
         *
         * @param region AWS region.
         * @param tableName table name
         * @return list of DynamoDB tables
         */
        AWSMOCK_API Entity::DynamoDb::ItemList ListItems(const std::string &region = {}, const std::string &tableName = {}) const;

        /**
         * @brief Returns a item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Item GetItemById(const std::string &oid);

        /**
         * @brief Creates a new item
         *
         * @param item DynamoDB item to create
         * @return created item
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Item CreateItem(const Entity::DynamoDb::Item &item);

        /**
         * @brief Updates an existing item
         *
         * @param item DynamoDB item to update
         * @return updated item
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Item UpdateItem(const Entity::DynamoDb::Item &item);

        /**
         * @brief Returns the number of DynamoDB items.
         *
         * @param region AWS region name
         * @return number of DynamoDB items
         */
        AWSMOCK_API long CountItems(const std::string &region = {}) const;

        /**
         * @brief Deletes an item
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @param keys primary key of the item
         */
        AWSMOCK_API void DeleteItem(const std::string &region, const std::string &tableName, const std::map<std::string, Entity::DynamoDb::AttributeValue> &keys);

        /**
         * @brief Deletes all item of a table
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @throws DatabaseException
         */
        AWSMOCK_API void DeleteItems(const std::string &region, const std::string &tableName);

        /**
         * @brief Deletes all items
         */
        AWSMOCK_API void DeleteAllItems();

      private:

        /**
         * Table map
         */
        std::map<std::string, Entity::DynamoDb::Table> _tables{};

        /**
         * Item map
         */
        std::map<std::string, Entity::DynamoDb::Item> _items{};

        /**
         * Table mutex
         */
        static boost::mutex _tableMutex;

        /**
         * Item mutex
         */
        static boost::mutex _itemMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_DYNAMODB_MEMORYDB_H
