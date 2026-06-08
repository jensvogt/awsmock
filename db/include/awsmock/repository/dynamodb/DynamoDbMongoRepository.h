//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/repository/dynamodb/DynamoDbToMongoTranslator.h>
#include <awsmock/repository/dynamodb/IDynamoDbRepository.h>
#include <awsmock/utils/ConnectionPool.h>
#include <awsmock/utils/SortColumn.h>

#define QUERY_SIZE_LIMIT (1024 * 1024)

namespace Awsmock::Database {

    /**
     * @brief DynamoDB MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbMongoRepository final : public IDynamoDbRepository {

    public:
        /**
         * @brief Constructor
         */
        explicit DynamoDbMongoRepository() = default;

        /**
         * @brief Check the existence of a DynamoDb table
         *
         * @param region AWS region name
         * @param tableName table name
         * @return true if the DynamoDb table already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool tableExists(const std::string &region, const std::string &tableName) const override;

        /**
         * @brief Create a new DynamoDb table
         *
         * @param table DynamoDb table
         * @return created DynamoDb table.
         */
        [[nodiscard]]
        Entity::DynamoDb::Table createTable(Entity::DynamoDb::Table &table) const override;

        /**
         * @brief Updates a DynamoDb table
         *
         * @param table DynamoDb table
         * @return updated DynamoDb table.
         */
        [[nodiscard]]
        Entity::DynamoDb::Table updateTable(Entity::DynamoDb::Table &table) const override;

        /**
         * @brief Create a new DynamoDb table or update an existing one
         *
         * @param table DynamoDb table
         * @return created or updated DynamoDb table.
         */
        [[nodiscard]]
        Entity::DynamoDb::Table createOrUpdateTable(Entity::DynamoDb::Table &table) const override;

        /**
         * @brief Update table counters
         *
         * @param tableArn table ARN
         * @param items number of items
         * @param size table size
         * @throws DatabaseException
         */
        void updateTableCounter(const std::string &tableArn, long items, long size) const override;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Table getTableById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param oid table primary key
         * @return table entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Table getTableById(const std::string &oid) const override;

        /**
         * @brief Returns a table entity by primary key
         *
         * @param region table region
         * @param name table name
         * @return table entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Table getTableByRegionName(const std::string &region, const std::string &name) const override;

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
        Entity::DynamoDb::TableList listTables(const std::string &region, const std::string &prefix, int pageSize, int pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns the number of DynamoDB tables
         *
         * @param region AWS region name
         * @param prefix table name prefix
         * @return number of DynamoDB tables
         */
        [[nodiscard]]
        long countTables(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Deletes an existing DynamoDB table
         *
         * @param region AWS region
         * @param tableName name of the table
         * @throws DatabaseException
         */
        void deleteTable(const std::string &region, const std::string &tableName) const override;

        /**
         * @brief Deletes all existing DynamoDB tables
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllTables() const override;

        /**
         * @brief Checks the existence of an item.
         *
         * @param item DynamoDB item
         * @return true if the database exists, otherwise false
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool itemExists(const Entity::DynamoDb::Item &item) const override;

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
        bool itemExists(const std::string &region, const std::string &tableName, std::string &partitionKey, const std::string &sortKey) const override;

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
        Entity::DynamoDb::ItemList listItems(const std::string &region, const std::string &tableName, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns an item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Item getItemById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns an item entity by primary key
         *
         * @param oid item primary key
         * @return item entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Item getItemById(const std::string &oid) const override;

        /**
         * @brief Returns an item entity by primary key
         *
         * @param region AWS region
         * @param tableName name of the table
         * @param partitionKey partition key
         * @param sortKey sort key, default = {}
         * @return item entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Item getItemByKeys(const std::string &region, const std::string &tableName, const Entity::DynamoDb::KeyValue &partitionKey, const Entity::DynamoDb::KeyValue &sortKey) const override;

        /**
         * @brief Creates a new item
         *
         * @param item DynamoDB item to create
         * @return created item
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Item createItem(Entity::DynamoDb::Item &item) const override;

        /**
         * @brief Updates an existing item
         *
         * @param item DynamoDB item to update
         * @return updated item
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Item updateItem(Entity::DynamoDb::Item &item) const override;

        /**
         * @brief Create or update an item
         *
         * @param item DynamoDB item to create or update
         * @return created or updated item
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::DynamoDb::Item createOrUpdateItem(Entity::DynamoDb::Item &item) const override;

        /**
         * @brief Returns the number of DynamoDB items.
         *
         * @param region AWS region name
         * @param tableName Dynamodb table name
         * @param prefix key prefix
         * @return number of DynamoDB items
         */
        [[nodiscard]]
        long countItems(const std::string &region, const std::string &tableName, const std::string &prefix) const override;

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
        std::vector<Entity::DynamoDb::Item> executeQuery(const DynamoToMongoTranslator::DynamoRequest &req, bool scanIndexForward, int limit) const override;

        /**
         * @brief Execute query
         *
         * @param filter query filter
         * @param scanIndexForward scan forward
         * @param limit query limit, default: 0
         * @return list of items
         * @throws DatabaseException
         */
        [[nodiscard]]
        std::vector<Entity::DynamoDb::Item> executeQuery(const value &filter, bool scanIndexForward, int limit = 0) const override;

        /**
         * @brief Adjust the item counters and updates the table entities.
         */
        void adjustItemCounters() const override;

        /**
         * @brief Deletes an item by its primary key
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @param partitionKey partition key
         * @param sortKey sort key, default = {}
         * @throws DatabaseException
         */
        void deleteItem(const std::string &region, const std::string &tableName, const std::string &partitionKey, const std::string &sortKey) const override;

        /**
         * @brief Deletes all item of a table
         *
         * @param region AWS region.
         * @param tableName name of the table
         * @return number of items deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteItems(const std::string &region, const std::string &tableName) const override;

        /**
         * @brief Deletes all items
         *
         * @return number of items deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllItems() const override;

        /**
         * @brief Get a list of all items of a table
         *
         * @param region AWS region
         * @param tableName table name
         * @return list of items;
         */
        [[nodiscard]]
        std::vector<Entity::DynamoDb::Item> getItems(const std::string &region, const std::string &tableName) const override;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "DynamoDB"};

        /**
         * @brief Dynamic attribute variants
         *
         * @param variant variant
         * @return key value
         */
        Entity::DynamoDb::KeyValue dynamoVariantToKeyValue(const Entity::DynamoDb::DynamoValue::DynamoVariant &variant) const;

        /**
         * @brief Dumps an item variant to stdout
         *
         * @param table name of the table
         * @param item item to dump
         */
        void dumpVariant(const Entity::DynamoDb::Table &table, Entity::DynamoDb::Item &item) const;

        /**
         * @brief Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * @brief Table collection name
         */
        static constexpr auto _tableCollectionName = "dynamodb_table";

        /**
         * @brief Item collection name
         */
        static constexpr auto _itemCollectionName = "dynamodb_item";

        /**
         * @brief AWS account ID
         */
        std::string _accountId;
    };

} // namespace Awsmock::Database
