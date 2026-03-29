//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/dynamodb/DynamoDbService.h>

#include "awsmock/dto/cognito/mapper/Mapper.h"
#include "awsmock/dto/dynamodb/internal/ExportItemsResponse.h"

namespace AwsMock::Service {

    Dto::DynamoDb::CreateTableResponse DynamoDbService::CreateTable(const Dto::DynamoDb::CreateTableRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "create_table");
        log_debug << "Start creating a new DynamoDb table, region: " << request.region << " name: " << request.tableName;

        Dto::DynamoDb::CreateTableResponse createTableResponse;
        createTableResponse.region = request.region;
        createTableResponse.tableName = request.tableName;

        if (_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_debug << "DynamoDb table exists already, region: " << request.region << " name: " << request.tableName;
            Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);
            createTableResponse.tableArn = table.arn;
            createTableResponse.keySchemas = Dto::DynamoDb::Mapper::map(table.keySchema);
            createTableResponse.attributeDefinitions = Dto::DynamoDb::Mapper::map(table.attributeDefinitions);
            return createTableResponse;
        }

        try {

            Database::Entity::DynamoDb::ProvisionedThroughput provisionedThroughput;
            provisionedThroughput.readCapacityUnits = request.provisionedThroughput.readCapacityUnits;
            provisionedThroughput.writeCapacityUnits = request.provisionedThroughput.writeCapacityUnits;
            Database::Entity::DynamoDb::Table table;
            table.region = request.region;
            table.name = request.tableName;
            table.arn = Core::AwsUtils::CreateDynamoDbTableArn(_accountId, request.tableName);
            table.provisionedThroughput = provisionedThroughput;
            table.status = Dto::DynamoDb::TableStatusTypeToString(Dto::DynamoDb::TableStatusType::ACTIVE);

            // Attributes
            if (!request.attributes.empty()) {
                for (const auto &a: request.attributes) {
                    table.attributeDefinitions.emplace_back(Dto::DynamoDb::Mapper::map(a));
                }
            }

            // Key schema
            if (!request.keySchema.empty()) {
                for (const auto &k: request.keySchema) {
                    table.keySchema.emplace_back(Dto::DynamoDb::Mapper::map(k));
                }
            }

            // Tags
            if (!request.tags.empty()) {
                for (const auto &k: request.tags) {
                    Database::Entity::DynamoDb::Tag tag;
                    tag.tagKey = k.tagKey;
                    tag.tagValue = k.tagValue;
                    table.tags.emplace_back(tag);
                }
            }

            table = _dynamoDbDatabase.CreateTable(table);
            createTableResponse.tableArn = table.arn;
            createTableResponse.keySchemas = Dto::DynamoDb::Mapper::map(table.keySchema);
            createTableResponse.attributeDefinitions = Dto::DynamoDb::Mapper::map(table.attributeDefinitions);
            log_debug << "DynamoDb table created, name: " << table.name;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd create table failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd create table failed, error: " + exc.message());
        }

        return createTableResponse;
    }

    bool DynamoDbService::ExistTable(const std::string &region, const std::string &tableName) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "table_exists");
        log_debug << "Starting exists table request, region: " << region << ", tableName: " << tableName;

        try {

            return _dynamoDbDatabase.TableExists(region, tableName);

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list tables failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd list tables failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::ListTableResponse DynamoDbService::ListTables(const Dto::DynamoDb::ListTableRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "list_tables");
        log_debug << "Starting list table request, region: " << request.region;

        try {

            const std::vector<Database::Entity::DynamoDb::Table> tables = _dynamoDbDatabase.ListTables(request.region);

            Dto::DynamoDb::ListTableResponse response;
            for (const auto &table: tables) {
                response.tableNames.emplace_back(table.name);
            }
            return response;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list tables failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd list tables failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::ListTableCountersResponse DynamoDbService::ListTableCounters(const Dto::DynamoDb::ListTableCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "list_table_counters");
        log_debug << "Starting list table request, region: " << request.region;

        try {

            Dto::DynamoDb::ListTableCountersResponse tableResponse;
            tableResponse.total = _dynamoDbDatabase.CountTables(request.region, request.prefix);
            for (std::vector<Database::Entity::DynamoDb::Table> tables = _dynamoDbDatabase.ListTables(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns)); const auto &table: tables) {
                Dto::DynamoDb::TableCounter tableCounter;
                tableCounter.region = table.region;
                tableCounter.tableName = table.name;
                tableCounter.tableArn = table.arn;
                tableCounter.items = table.itemCount;
                tableCounter.size = table.size;
                tableCounter.created = table.created;
                tableCounter.modified = table.modified;
                tableResponse.tableCounters.push_back(tableCounter);
            }
            return tableResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list table counters failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd list table counters failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::GetTableDetailCountersResponse DynamoDbService::GetTableDetailCounters(const Dto::DynamoDb::GetTableDetailCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "get_table_detail_counters");
        log_debug << "Starting get table detail request, region: " << request.region << ", tableName: " << request.tableName;

        try {

            Dto::DynamoDb::GetTableDetailCountersResponse tableResponse;
            const Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);
            tableResponse.tableCounters.region = table.region;
            tableResponse.tableCounters.tableName = table.name;
            tableResponse.tableCounters.items = table.itemCount;
            tableResponse.tableCounters.size = table.size;
            tableResponse.tableCounters.status = table.status;
            tableResponse.tableCounters.created = table.created;
            tableResponse.tableCounters.modified = table.modified;
            return tableResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd get table detail counters failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd get table detail counters failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::ListTableArnsResponse DynamoDbService::ListTableArns(const std::string &region) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "list_table_arns");
        log_debug << "Starting list table arns request, region: " << region;

        try {

            Dto::DynamoDb::ListTableArnsResponse tableResponse;
            for (const std::vector<Database::Entity::DynamoDb::Table> tables = _dynamoDbDatabase.ListTables(region); const auto &table: tables) {
                tableResponse.tableArns.emplace_back(table.arn);
            }
            return tableResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list table arns failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd list table arns failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::ListStreamsResponse DynamoDbService::ListStreams(const Dto::DynamoDb::ListStreamsRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "list_streams");
        log_debug << "Starting list streams request, region: " << request.region;

        try {

            // Send request to docker container
            /*            std::map<std::string, std::string> headers = PrepareStreamHeaders("ListStreams");
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.ToJson(), headers);
            Dto::DynamoDb::ListStreamsResponse listTableResponse = Dto::DynamoDb::ListStreamsResponse::FromJson(body);*/
            return {};

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list table counters failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb list table counters failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::ListItemCountersResponse DynamoDbService::ListItemCounters(const Dto::DynamoDb::ListItemCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "list_item_counters");
        log_debug << "Starting list item request, region: " << request.region;

        try {
            Dto::DynamoDb::ListItemCountersResponse itemResponse;
            itemResponse.total = _dynamoDbDatabase.CountItems(request.region, request.tableName, request.prefix);
            itemResponse.itemCounters = Dto::DynamoDb::Mapper::mapCounter(_dynamoDbDatabase.ListItems(request.region, request.tableName));
            return itemResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list item counters failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd list item counters failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::DescribeTableResponse DynamoDbService::DescribeTable(const Dto::DynamoDb::DescribeTableRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "describe_table");
        log_debug << "Describe DynamoDb table, region: " << request.region << " name: " << request.tableName;

        try {
            const Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);
            Dto::DynamoDb::DescribeTableResponse response;
            response.region = request.region;
            response.tableName = request.tableName;
            response.itemCount = table.itemCount;
            response.tableArn = table.arn;
            response.tableStatus = Dto::DynamoDb::TableStatusTypeFromString(table.status);

            // Attribute definitions
            if (!table.attributeDefinitions.empty()) {
                for (const auto &a: table.attributeDefinitions) {
                    response.attributeDefinitions.emplace_back(Dto::DynamoDb::Mapper::map(a));
                }
            }

            // Key Schema
            if (!table.keySchema.empty()) {
                for (const auto &k: table.keySchema) {
                    response.keySchema.emplace_back(Dto::DynamoDb::Mapper::map(k));
                }
            }

            // Tags
            if (!table.tags.empty()) {
                for (const auto &t: table.tags) {
                    Dto::DynamoDb::Tag tag;
                    tag.tagKey = t.tagKey;
                    tag.tagValue = t.tagValue;
                    response.tags.emplace_back(tag);
                }
            }

            log_debug << "DynamoDb describe table, name: " << request.tableName;
            return response;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb describe table failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb describe table failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::DeleteTableResponse DynamoDbService::DeleteTable(const Dto::DynamoDb::DeleteTableRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "delete_table");
        log_debug << "Delete DynamoDb table, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table does not exist, region: " + request.region + " name: " + request.tableName);
        }

        try {
            // Get the table
            const Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);

            // Delete all items
            const long deleted = _dynamoDbDatabase.DeleteItems(request.region, request.tableName);
            log_debug << "Items deleted, table: " << request.tableName << ", count: " << deleted;

            // Delete table in a database
            _dynamoDbDatabase.DeleteTable(request.region, request.tableName);
            log_debug << "DynamoDb table deleted, name: " << request.tableName;

            Dto::DynamoDb::DeleteTableResponse deleteTableResponse = {};
            deleteTableResponse.region = table.region;
            deleteTableResponse.tableId = table.oid;
            deleteTableResponse.tableName = table.name;
            deleteTableResponse.tableArn = table.arn;
            deleteTableResponse.itemCount = deleted;
            return deleteTableResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete table failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete table failed, error: " + exc.message());
        }
    }

    long DynamoDbService::DeleteAllTables() const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "delete_all_tables");
        log_debug << "Deleting all tables";

        try {

            // Delete all items
            const long itemCount = _dynamoDbDatabase.DeleteAllItems();
            log_debug << "Items deleted, count: " << itemCount;

            // Delete the table from the database
            const long count = _dynamoDbDatabase.DeleteAllTables();
            log_debug << "DynamoDb tables deleted, count: " << count;
            return count;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete table failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete table failed, message: " + exc.message());
        }
    }

    Dto::DynamoDb::GetItemResponse DynamoDbService::GetItem(Dto::DynamoDb::GetItemRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "get_item");
        log_debug << "Start get item, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        try {

            // Get the table and the primary keys
            const Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);
            std::string partitionKey = request.keys[table.GetPartitionKeyName()].stringValue;
            std::string sortKey = request.keys[table.GetSortKeyName()].stringValue;

            if (!_dynamoDbDatabase.ItemExists(request.region, request.tableName, partitionKey, sortKey)) {
                log_debug << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
                Dto::DynamoDb::GetItemResponse getItemResponse;
                getItemResponse.region = request.region;
                getItemResponse.user = request.user;
                getItemResponse.requestId = request.requestId;
                return getItemResponse;
            }

            // Get item
            const Database::Entity::DynamoDb::Item item = _dynamoDbDatabase.GetItemByKeys(request.region, request.tableName, partitionKey, sortKey);

            // Prepare response
            Dto::DynamoDb::GetItemResponse getItemResponse;
            getItemResponse.region = request.region;
            getItemResponse.user = request.user;
            getItemResponse.requestId = request.requestId;
            getItemResponse.attributes = Dto::DynamoDb::Mapper::map(item.attributes);
            return getItemResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd get item failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd get item failed, , error: " + exc.message());
        }
    }

    Dto::DynamoDb::PutItemResponse DynamoDbService::PutItem(Dto::DynamoDb::PutItemRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "put_item");
        log_debug << "Start put item, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table does not exist, region: " + request.region + " name: " + request.tableName);
        }

        try {

            Database::Entity::DynamoDb::Item item = Dto::DynamoDb::Mapper::map(request);
            item.size = sizeof(item) + sizeof(long);

            // Get the table
            Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);
            item.partitionKey = request.attributes[table.GetPartitionKeyName()].stringValue;
            item.sortKey = request.attributes[table.GetSortKeyName()].stringValue;

            item = _dynamoDbDatabase.CreateOrUpdateItem(item);
            log_debug << "DynamoDb put item, region: " << item.region << " tableName: " << item.tableName;

            // Update table
            table.size += item.size;
            table.itemCount++;
            table = _dynamoDbDatabase.UpdateTable(table);
            log_debug << "Database updated, region: " << table.region << " tableName: " << table.name;

            Dto::DynamoDb::PutItemResponse response;
            response.consumedCapacity.tableName = item.tableName;
            response.item.region = item.region;
            response.item.tableName = item.tableName;
            response.item.attributes = Dto::DynamoDb::Mapper::map(item.attributes);
            response.item.attributes[table.GetPartitionKeyName()] = Dto::DynamoDb::Mapper::map(item.partitionKey);
            response.item.attributes[table.GetSortKeyName()] = Dto::DynamoDb::Mapper::map(item.sortKey);
            response.item.created = item.created;
            response.item.modified = item.modified;
            return response;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb put item failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb put item failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::QueryResponse DynamoDbService::Query(Dto::DynamoDb::QueryRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "query");
        log_debug << "Start query, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        try {

            // Add table name
            std::string expression = CreateExpression(request.keyConditionExpression);
            ExpressionAttributeValues attrs = CreateAttributeValues(request.tableName, request.expressionAttributeValues);

            // Create a MongoDB filter from DynamodDb query
            auto filter = ToMongoFilter(expression, attrs);
            log_info << "MongoDB query: " << bsoncxx::to_json(filter);

            // Query database
            std::vector<Database::Entity::DynamoDb::Item> items = _dynamoDbDatabase.ExecuteQuery(filter, true, request.limit);

            // Prepare response
            Dto::DynamoDb::QueryResponse queryResponse;
            queryResponse.tableName = request.tableName;
            for (const auto &item: items) {
                queryResponse.items.emplace_back(Dto::DynamoDb::Mapper::map(item.attributes));
            }
            return queryResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb query failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb query failed, error: " + exc.message());
        } catch (Core::DynamoDbParseException &exc) {
            log_error << "DynamoDb query parsing failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb query parsing failed, error: " + std::string(exc.message()));
        }
    }

    Dto::DynamoDb::ScanResponse DynamoDbService::Scan(const Dto::DynamoDb::ScanRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "scan");
        log_debug << "Start scan, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        try {
            const std::vector<Database::Entity::DynamoDb::Item> items = _dynamoDbDatabase.ListItems(request.region, request.tableName);
            const long count = _dynamoDbDatabase.CountItems(request.region, request.tableName);

            Dto::DynamoDb::ScanResponse scanResponse;
            scanResponse.region = request.region;
            scanResponse.user = request.user;
            scanResponse.requestId = request.requestId;
            scanResponse.tableName = request.tableName;
            scanResponse.count = count;
            scanResponse.scannedCount = static_cast<long>(items.size());
            for (const auto &item: items) {
                scanResponse.items.emplace_back(Dto::DynamoDb::Mapper::map(item.attributes));
            }
            return scanResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb scan failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
    }

    Dto::DynamoDb::DeleteItemResponse DynamoDbService::DeleteItem(Dto::DynamoDb::DeleteItemRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "delete_item");
        log_debug << "Start creating a new DynamoDb item, region: " << request.region << " table: " << request.tableName;

        if (const Database::Entity::DynamoDb::Item item = Dto::DynamoDb::Mapper::map(request); !_dynamoDbDatabase.ItemExists(item)) {
            log_warning << "DynamoDb item does not exist, region: " << request.region << " name: " << request.tableName;
            return {};
        }

        try {

            // Get the table
            const Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);
            std::string partitionKey = request.keys[table.GetPartitionKeyName()].stringValue;
            std::string sortKey = request.keys[table.GetSortKeyName()].stringValue;

            // Get the item
            const Database::Entity::DynamoDb::Item item = _dynamoDbDatabase.GetItemByKeys(request.region, request.tableName, item.partitionKey, item.sortKey);

            // Delete item
            _dynamoDbDatabase.DeleteItem(request.region, request.tableName, partitionKey, sortKey);

            Dto::DynamoDb::DeleteItemResponse deleteItemResponse;
            deleteItemResponse.requestId = request.requestId;
            deleteItemResponse.region = request.region;
            deleteItemResponse.user = request.user;
            deleteItemResponse.attributes = Dto::DynamoDb::Mapper::map(item.attributes);
            log_debug << "DynamoDb item deleted, table: " << request.tableName;
            return deleteItemResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete item failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete item failed, message: " + exc.message());
        }
    }

    void DynamoDbService::DeleteAllItems(const Dto::DynamoDb::DeleteAllItemsRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "delete_all_items");
        log_debug << "Start deleting all items, region: " << request.region << " table: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table does not exist, region: " + request.region + " name: " + request.tableName);
        }

        try {

            // Delete table
            Dto::DynamoDb::DeleteTableRequest delRequest;
            delRequest.tableName = request.tableName;
            delRequest.region = request.region;
            Dto::DynamoDb::DeleteTableResponse delResponse = DeleteTable(delRequest);

            Dto::DynamoDb::CreateTableRequest createRequest;
            createRequest.tableName = request.tableName;
            createRequest.region = request.region;
            Dto::DynamoDb::CreateTableResponse createResponse = CreateTable(createRequest);
            log_debug << "DynamoDb item deleted, table: " << request.tableName;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete item failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete item failed, message: " + exc.message());
        }
    }

    Dto::DynamoDb::ExportItemsResponse DynamoDbService::ExportItems(const Dto::DynamoDb::ExportItemsRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "export_items");
        log_debug << "Start exporting all items, region: " << request.region << " table: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table does not exist, region: " + request.region + " name: " + request.tableName);
        }

        try {

            // Get items
            Dto::DynamoDb::ExportItemsResponse response;
            response.tableName = request.tableName;
            response.region = request.region;
            response.infrastructure.dynamoDbItems = _dynamoDbDatabase.GetItems(request.region, request.tableName);
            return response;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete item failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete item failed, message: " + exc.message());
        }
    }

    void DynamoDbService::ResetItemCounters() const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "reset_item_counters");
        log_debug << "Reset item counters";

        try {

            // Recalculate item counters
            _dynamoDbDatabase.AdjustItemCounters();

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd reset item counters failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd reset item counters failed, message: " + exc.message());
        }
    }

    std::string DynamoDbService::CreateExpression(const std::string &inputExpression) {
        std::string expression = Core::StringUtils::ReplaceCopy(inputExpression, "AMZN_MAPPED_", "");
        expression = Core::StringUtils::ReplaceCopy(expression, "#", "");
        return "tableName = :tableName AND " + expression;
    }

    ExpressionAttributeValues DynamoDbService::CreateAttributeValues(const std::string &tableName, std::map<std::string, Dto::DynamoDb::AttributeValue> expressionAttributeValues) {

        ExpressionAttributeValues attr;
        for (auto &[fst, snd]: expressionAttributeValues) {
            attr[Core::StringUtils::ReplaceCopy(fst, "AMZN_MAPPED_", "")] = snd;
        }
        Dto::DynamoDb::AttributeValue tableAttributeValue;
        tableAttributeValue.type = "S";
        tableAttributeValue.stringValue = tableName;
        attr[":tableName"] = tableAttributeValue;
        return attr;
    }

}// namespace AwsMock::Service
