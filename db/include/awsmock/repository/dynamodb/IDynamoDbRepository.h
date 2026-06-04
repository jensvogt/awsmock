//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/dynamodb/Item.h>
#include <awsmock/entity/dynamodb/Table.h>
#include <awsmock/repository/dynamodb/DynamoDbToMongoTranslator.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for SQS repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * SQS-related data.
     */
    class IDynamoDbRepository {

      public:

        /**
         * @brief Virtual destructor for the ISQSRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~IDynamoDbRepository() = default;

        /**
         * @brief Check the existence of a DynamoDb table
         *
         * @param region AWS region name
         * @param tableName table name
         * @return true if the DynamoDb table already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool tableExists(const std::string &region, const std::string &tableName) const = 0;

        /**
         * @brief Create a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return created DynamoDb table.
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Table createTable(Entity::DynamoDb::Table &table) const = 0;

        /**
         * @brief Updates a DynamoDb table
         *
         * @param table DynamoDb table
         * @return updated DynamoDb table.
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Table updateTable(Entity::DynamoDb::Table &table) const = 0;

        /**
         * @brief Create a new DynamoDb table or update an existing one
         *
         * @param table DynamoDb table
         * @return created or updated DynamoDb table.
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Table createOrUpdateTable(Entity::DynamoDb::Table &table) const = 0;

        /**
         * @brief Update table counters
         *
         * @param tableArn table ARN
         * @param items number of items
         * @param size table size
         * @throws DatabaseException
         */
        virtual void updateTableCounter(const std::string &tableArn, long items, long size) const = 0;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Table getTableById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Table getTableById(const std::string &oid) const = 0;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param region table region
         * @param name table name
         * @return table entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Table getTableByRegionName(const std::string &region, const std::string &name) const = 0;

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
        [[nodiscard]]
        virtual Entity::DynamoDb::TableList listTables(const std::string &region, const std::string &prefix, int pageSize, int pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns the number of DynamoDB tables
         *
         * @param region AWS region name
         * @param prefix table name prefix
         * @return number of DynamoDB tables
         */
        [[nodiscard]]
        virtual long countTables(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Deletes an existing DynamoDB table
         *
         * @param region AWS region
         * @param tableName name of the table
         * @throws DatabaseException
         */
        virtual void deleteTable(const std::string &region, const std::string &tableName) const = 0;

        /**
         * @brief Deletes all existing DynamoDB tables
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllTables() const = 0;

        /**
         * @brief Checks the existence of an item.
         *
         * @param item DynamoDB item
         * @return true if the database exists, otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool itemExists(const Entity::DynamoDb::Item &item) const = 0;

        /**
         * @brief Checks the existence of an item.
         *
         * @param region AWS region
         * @param tableName name of the table
         * @param partitionKey
         * @param sortKey
         * @param partitionKey
         * @param sortKey
         * @return true if the database exists, otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool itemExists(const std::string &region, const std::string &tableName, std::string &partitionKey, const std::string &sortKey) const = 0;

        /**
         * @brief Returns a list of DynamoDB items
         *
         * @param region AWS region.
         * @param tableName table name
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sort columns
         * @return list of DynamoDB tables
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::ItemList listItems(const std::string &region, const std::string &tableName, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns an item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Item getItemById(const bsoncxx::oid &oid) const = 0;


        /**
         * @brief Returns an item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Item getItemById(const std::string &oid) const = 0;

        /**
         * @brief Returns an item entity by primary key
         *
         * @param region AWS region
         * @param tableName name of the table
         * @param partitionKey partition key
         * @param sortKey sort key
         * @return item entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Item getItemByKeys(const std::string &region, const std::string &tableName, const Entity::DynamoDb::KeyValue &partitionKey, const Entity::DynamoDb::KeyValue &sortKey) const = 0;

        /**
         * @brief Creates a new item
         *
         * @param item DynamoDB item to create
         * @return created item
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Item createItem(Entity::DynamoDb::Item &item) const = 0;

        /**
         * @brief Updates an existing item
         *
         * @param item DynamoDB item to update
         * @return updated item
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Item updateItem(Entity::DynamoDb::Item &item) const = 0;

        /**
         * @brief Create or update an item
         *
         * @param item DynamoDB item to create or update
         * @return created or updated item
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::DynamoDb::Item createOrUpdateItem(Entity::DynamoDb::Item &item) const = 0;

        /**
         * @brief Get a list of all items of a table
         *
         * @param region AWS region
         * @param tableName table name
         * @return list of items;
         */
        [[nodiscard]]
        virtual std::vector<Entity::DynamoDb::Item> getItems(const std::string &region, const std::string &tableName) const = 0;

        /**
         * @brief Returns the number of DynamoDB items.
         *
         * @param region AWS region name
         * @param tableName Dynamodb table name
         * @param prefix key prefix
         * @return number of DynamoDB items
         */
        [[nodiscard]]
        virtual long countItems(const std::string &region, const std::string &tableName, const std::string &prefix) const = 0;

        /**
         * @brief Execute a query
         *
         * @param req request
         * @param scanIndexForward scan forward
         * @param limit query limit
         * @return list of items
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual std::vector<Entity::DynamoDb::Item> executeQuery(const DynamoToMongoTranslator::DynamoRequest &req, bool scanIndexForward, int limit) const = 0;

        /**
         * @brief Execute a query
         *
         * @param filter query filter
         * @param scanIndexForward scan forward
         * @param limit query limit, default: 0
         * @return list of items
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual std::vector<Entity::DynamoDb::Item> executeQuery(const value &filter, bool scanIndexForward, int limit = 0) const = 0;

        /**
         * @brief Adjust the item counters and updates the table entities.
         */
        virtual void adjustItemCounters() const = 0;

        /**
         * @brief Deletes an item by its primary key
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @param partitionKey partition key
         * @param sortKey sort key, default = {}
         * @throws DatabaseException
         */
        virtual void deleteItem(const std::string &region, const std::string &tableName, const std::string &partitionKey, const std::string &sortKey) const = 0;

        /**
         * @brief Deletes all item of a table
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @return number of items deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteItems(const std::string &region, const std::string &tableName) const = 0;

        /**
         * @brief Deletes all items
         *
         * @return number of items deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllItems() const = 0;
    };

}// namespace Awsmock::Database