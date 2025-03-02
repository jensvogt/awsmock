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
#include <awsmock/core/Macros.h>
#include <awsmock/core/SortColumn.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/memorydb/DynamoDbMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Dto::DynamoDb {
    struct Key;
}
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
        explicit AWSMOCK_API DynamoDbDatabase();

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
        AWSMOCK_API bool TableExists(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Create a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return created DynamoDb table.
         */
        AWSMOCK_API Entity::DynamoDb::Table CreateTable(Entity::DynamoDb::Table &table) const;

        /**
         * @brief Updates a DynamoDb table
         *
         * @param table DynamoDb table
         * @return updated DynamoDb table.
         */
        AWSMOCK_API Entity::DynamoDb::Table UpdateTable(Entity::DynamoDb::Table &table) const;

        /**
         * @brief Create a new DynamoDb table or update an existing one
         *
         * @param table DynamoDb table
         * @return created or updated DynamoDb table.
         */
        AWSMOCK_API Entity::DynamoDb::Table CreateOrUpdateTable(Entity::DynamoDb::Table &table) const;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Table GetTableById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Table GetTableById(const std::string &oid) const;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param region table region
         * @param name table name
         * @return table entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Table GetTableByRegionName(const std::string &region, const std::string &name) const;

        /**
         * @brief Returns a list of DynamoDB tables
         *
         * @param region AWS region name
         * @param prefix table name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of DynamoDB tables
         */
        AWSMOCK_API Entity::DynamoDb::TableList ListTables(const std::string &region = {}, const std::string &prefix = {}, int pageSize = 0, int pageIndex = 0, const std::vector<Core::SortColumn> &sortColumns = {}) const;

        /**
         * @brief Returns the number of DynamoDB tables
         *
         * @param region AWS region name
         * @param prefix table name prefix
         * @return number of DynamoDB tables
         */
        AWSMOCK_API long CountTables(const std::string &region = {}, const std::string &prefix = {}) const;

        /**
         * @brief Deletes an existing DynamoDB table
         *
         * @param region AWS region
         * @param tableName name of the table
         * @throws DatabaseException
         */
        AWSMOCK_API void DeleteTable(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Deletes all existing DynamoDB tables
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        AWSMOCK_API long DeleteAllTables() const;

        /**
         * @brief Checks the existence of an item.
         *
         * @param item DynamoDB item
         * @return true if database exists, otherwise false
         * @throws DatabaseException
         */
        AWSMOCK_API bool ItemExists(const Entity::DynamoDb::Item &item) const;

        /**
         * @brief Returns a list of DynamoDB items
         *
         * @param region AWS region.
         * @param tableName table name
         * @return list of DynamoDB tables
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::ItemList ListItems(const std::string &region = {}, const std::string &tableName = {}) const;


        /**
         * @brief Returns a item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Item GetItemById(bsoncxx::oid oid) const;

        /**
         * @brief Creates a new item
         *
         * @param item DynamoDB item to create
         * @return created item
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Item CreateItem(Entity::DynamoDb::Item &item) const;

        /**
         * @brief Updates an existing item
         *
         * @param item DynamoDB item to update
         * @return updated item
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Item UpdateItem(Entity::DynamoDb::Item &item) const;

        /**
         * @brief Create or update item
         *
         * @param item DynamoDB item to create or update
         * @return created or updated item
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::DynamoDb::Item CreateOrUpdateItem(Entity::DynamoDb::Item &item) const;

        /**
         * @brief Returns the number of DynamoDB items.
         *
         * @param region AWS region name
         * @param tableName Dynamodb table name
         * @param prefix key prefix
         * @return number of DynamoDB items
         */
        AWSMOCK_API long CountItems(const std::string &region = {}, const std::string &tableName = {}, const std::string &prefix = {}) const;

        /**
         * @brief Deletes an item by primary key
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @param keys primary key of the item
         * @throws DatabaseException
         */
        AWSMOCK_API void DeleteItem(const std::string &region, const std::string &tableName, const std::map<std::string, Entity::DynamoDb::AttributeValue> &keys) const;

        /**
         * @brief Deletes all item of a table
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @throws DatabaseException
         */
        AWSMOCK_API void DeleteItems(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Deletes all items
         *
         * @throws DatabaseException
         */
        AWSMOCK_API void DeleteAllItems() const;

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
