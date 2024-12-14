//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_DYNAMODB_DATABASE_H
#define AWSMOCK_REPOSITORY_DYNAMODB_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/memorydb/DynamoDbMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief DynamoDB MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit DynamoDbDatabase();

        /**
         * @brief Singleton instance
         */
        static DynamoDbDatabase &instance() {
            static DynamoDbDatabase dynamoDbDatabase;
            return dynamoDbDatabase;
        }

        /**
         * @brief Check existence of DynamoDb table
         *
         * @param region AWS region name
         * @param tableName table name
         * @return true if DynamoDb table already exists
         * @throws DatabaseException
         */
        bool TableExists(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Create a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return created DynamoDb table.
         */
        Entity::DynamoDb::Table CreateTable(const Entity::DynamoDb::Table &table) const;

        /**
         * @brief Updates a DynamoDb table
         *
         * @param table DynamoDb table
         * @return updated DynamoDb table.
         */
        Entity::DynamoDb::Table UpdateTable(Entity::DynamoDb::Table &table) const;

        /**
         * @brief Create a new DynamoDb table or update an existing one
         *
         * @param table DynamoDb table
         * @return created or updated DynamoDb table.
         */
        Entity::DynamoDb::Table CreateOrUpdateTable(Entity::DynamoDb::Table &table) const;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        Entity::DynamoDb::Table GetTableById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        Entity::DynamoDb::Table GetTableById(const std::string &oid) const;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param region table region
         * @param name table name
         * @return table entity
         * @throws DatabaseException
         */
        Entity::DynamoDb::Table GetTableByRegionName(const std::string &region, const std::string &name) const;

        /**
         * @brief Returns a list of DynamoDB tables
         *
         * @param region AWS region name
         * @return list of DynamoDB tables
         */
        Entity::DynamoDb::TableList ListTables(const std::string &region = {}) const;

        /**
         * @brief Returns the number of DynamoDB tables
         *
         * @param region AWS region name
         * @return number of DynamoDB tables
         */
        long CountTables(const std::string &region = {}) const;

        /**
         * @brief Deletes an existing DynamoDB table
         *
         * @param region AWS region
         * @param tableName name of the table
         * @throws DatabaseException
         */
        void DeleteTable(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Deletes all existing DynamoDB tables
         *
         * @throws DatabaseException
         */
        void DeleteAllTables() const;

        /**
         * @brief Checks the existence of an item.
         *
         * @param item DynamoDB item
         * @return true if database exists, otherwise false
         * @throws DatabaseException
         */
        bool ItemExists(const Entity::DynamoDb::Item &item) const;

        /**
         * @brief Returns a list of DynamoDB items
         *
         * @param region AWS region.
         * @param tableName table name
         * @return list of DynamoDB tables
         * @throws DatabaseException
         */
        Entity::DynamoDb::ItemList ListItems(const std::string &region = {}, const std::string &tableName = {}) const;


        /**
         * @brief Returns a item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        Entity::DynamoDb::Item GetItemById(bsoncxx::oid oid) const;

        /**
         * @brief Creates a new item
         *
         * @param item DynamoDB item to create
         * @return created item
         * @throws DatabaseException
         */
        Entity::DynamoDb::Item CreateItem(const Entity::DynamoDb::Item &item) const;

        /**
         * @brief Updates an existing item
         *
         * @param item DynamoDB item to update
         * @return updated item
         * @throws DatabaseException
         */
        Entity::DynamoDb::Item UpdateItem(const Entity::DynamoDb::Item &item) const;

        /**
         * @brief Create or update item
         *
         * @param item DynamoDB item to create or update
         * @return created or updated item
         * @throws DatabaseException
         */
        Entity::DynamoDb::Item CreateOrUpdateItem(const Entity::DynamoDb::Item &item) const;

        /**
         * @brief Returns the number of DynamoDB items.
         *
         * @param region AWS region name
         * @return number of DynamoDB items
         */
        long CountItems(const std::string &region = {}) const;

        /**
         * @brief Deletes an item by primary key
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @param key primary key of the item
         * @throws DatabaseException
         */
        void DeleteItem(const std::string &region, const std::string &tableName, const std::string &key) const;

        /**
         * @brief Deletes all item of a table
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @throws DatabaseException
         */
        void DeleteItems(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Deletes all items
         *
         * @throws DatabaseException
         */
        void DeleteAllItems() const;

      private:

        /**
         * @brief Database name
         */
        std::string _databaseName;

        /**
         * @brief Table collection name
         */
        std::string _tableCollectionName;

        /**
         * @brief Item collection name
         */
        std::string _itemCollectionName;

        /**
         * @brief DynamoDB in-memory database
         */
        DynamoDbMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_DYNAMODB_DATABASE_H
