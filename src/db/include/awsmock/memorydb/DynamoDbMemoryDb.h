//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_DYNAMODB_MEMORYDB_H
#define AWSMOCK_REPOSITORY_DYNAMODB_MEMORYDB_H

// C++ standard includes
#include <iostream>
#include <string>
#include <vector>

// Poco includes
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <Poco/UUIDGenerator.h>

// AwsMock includes
#include "awsmock/core/config/Configuration.h"
#include <awsmock/core/DirUtils.h>
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/LogStream.h>
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
        DynamoDbMemoryDb() = default;

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
        bool TableExists(const std::string &region, const std::string &tableName);

        /**
         * @brief Create a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return created DynamoDb table.
         */
        Entity::DynamoDb::Table CreateTable(const Entity::DynamoDb::Table &table);

        /**
         * @brief Updates a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return updated DynamoDb table.
         */
        Entity::DynamoDb::Table UpdateTable(const Entity::DynamoDb::Table &table);

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        Entity::DynamoDb::Table GetTableById(const std::string &oid);

        /**
         * @brief Returns a table entity by region and name
         *
         * @param region table region
         * @param name table name
         * @return table entity
         * @throws DatabaseException
         */
        Entity::DynamoDb::Table GetTableByRegionName(const std::string &region, const std::string &name);

        /**
         * @brief Returns a list of DynamoDB tables
         *
         * @param region AWS region name
         * @return list of DynamoDB tables
         */
        std::vector<Entity::DynamoDb::Table> ListTables(const std::string &region = {});

        /**
         * @brief Returns the number of DynamoDB tables
         *
         * @param region AWS region name
         * @return number of DynamoDB tables
         */
        long CountTables(const std::string &region = {});

        /**
         * @brief Deletes an existing DynamoDB table
         *
         * @param tableName name of the table
         * @throws DatabaseException
         */
        void DeleteTable(const std::string &tableName);

        /**
         * @brief Deletes all existing DynamoDB tables
         *
         * @throws DatabaseException
         */
        void DeleteAllTables();

        /**
         * @brief Checks the existence of an item.
         *
         * @param item DynamoDB item
         * @return true if database exists, otherwise false
         * @throws DatabaseException
         */
        bool ItemExists(const Entity::DynamoDb::Item &item);

        /**
         * @brief Returns a list of DynamoDB items
         *
         * @param region AWS region.
         * @param tableName table name
         * @return list of DynamoDB tables
         */
        Entity::DynamoDb::ItemList ListItems(const std::string &region = {}, const std::string &tableName = {});

        /**
         * @brief Returns a item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        Entity::DynamoDb::Item GetItemById(const std::string &oid);

        /**
         * @brief Creates a new item
         *
         * @param item DynamoDB item to create
         * @return created item
         * @throws DatabaseException
         */
        Entity::DynamoDb::Item CreateItem(const Entity::DynamoDb::Item &item);

        /**
         * @brief Updates an existing item
         *
         * @param item DynamoDB item to update
         * @return updated item
         * @throws DatabaseException
         */
        Entity::DynamoDb::Item UpdateItem(const Entity::DynamoDb::Item &item);

        /**
         * @brief Returns the number of DynamoDB items.
         *
         * @param region AWS region name
         * @return number of DynamoDB items
         */
        long CountItems(const std::string &region = {});

        /**
         * @brief Deletes an item
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @param key primary key of the item
         */
        void DeleteItem(const std::string &region, const std::string &tableName, const std::string &key);

        /**
         * @brief Deletes all item of a table
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @throws DatabaseException
         */
        void DeleteItems(const std::string &region, const std::string &tableName);

        /**
         * @brief Deletes all items
         */
        void DeleteAllItems();

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
        static Poco::Mutex _tableMutex;

        /**
         * Item mutex
         */
        static Poco::Mutex _itemMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_DYNAMODB_MEMORYDB_H
