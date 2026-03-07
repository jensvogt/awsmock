//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/dynamodb/DynamoDbService.h>

namespace AwsMock::Service {

    DynamoDbService::DynamoDbService() : _dynamoDbDatabase(Database::DynamoDbDatabase::instance()) {

        // DynamoDB docker host, port
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        _containerHost = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.dynamodb.container.host");
        _containerPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.dynamodb.container.port");
    }

    Dto::DynamoDb::CreateTableResponse DynamoDbService::CreateTable(const Dto::DynamoDb::CreateTableRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "create_table");
        log_debug << "Start creating a new DynamoDb table, region: " << request.region << " name: " << request.tableName;

        Dto::DynamoDb::CreateTableResponse createTableResponse;
        createTableResponse.region = request.region;
        createTableResponse.tableName = request.tableName;

        if (_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_debug << "DynamoDb table exists already, region: " << request.region << " name: " << request.tableName;
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
            std::map<std::string, std::string> headers = PrepareStreamHeaders("ListStreams");
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.ToJson(), headers);
            Dto::DynamoDb::ListStreamsResponse listTableResponse = Dto::DynamoDb::ListStreamsResponse::FromJson(body);
            return listTableResponse;

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

            // Send request to DynamoDB docker container
            //            std::map<std::string, std::string> headers = PrepareHeaders("DeleteTable");
            //            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.ToJson(), headers);

            const Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);

            // Delete all items
            const long deleted = _dynamoDbDatabase.DeleteItems(request.region, request.tableName);
            log_debug << "Items deleted, table: " << request.tableName << ", count: " << deleted;

            // Delete table in a database
            _dynamoDbDatabase.DeleteTable(request.region, request.tableName);
            log_debug << "DynamoDb table deleted, name: " << request.tableName;

            Dto::DynamoDb::DeleteTableResponse deleteTableResponse = {};
            deleteTableResponse.region = table.region;
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

            // Delete all tables from DynamoDB
            for (const auto &table: _dynamoDbDatabase.ListTables()) {

                std::map<std::string, std::string> headers = PrepareHeaders("DeleteTable");
                Dto::DynamoDb::DeleteTableRequest dynamoDeleteRequest;
                dynamoDeleteRequest.tableName = table.name;
                SendAuthorizedDynamoDbRequest(dynamoDeleteRequest.ToJson(), headers);
            }

            // Delete the table from the database
            const long count = _dynamoDbDatabase.DeleteAllTables();
            log_debug << "DynamoDb tables deleted, count: " << count;
            return count;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete table failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete table failed, message: " + exc.message());
        }
    }

    Dto::DynamoDb::GetItemResponse DynamoDbService::GetItem(const Dto::DynamoDb::GetItemRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "get_item");
        log_debug << "Start get item, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        try {
            const Database::Entity::DynamoDb::Item item = _dynamoDbDatabase.GetItemByKeys(request.region, request.tableName, Dto::DynamoDb::Mapper::map(request.keys));

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
        return {};
    }

    Dto::DynamoDb::PutItemResponse DynamoDbService::PutItem(Dto::DynamoDb::PutItemRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "put_item");
        log_info << "Start put item, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table does not exist, region: " + request.region + " name: " + request.tableName);
        }

        try {

            Database::Entity::DynamoDb::Item item = Dto::DynamoDb::Mapper::map(request);
            item.size = sizeof(item) + sizeof(long);

            // Key schema
            Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);
            for (auto &k: table.keySchema) {
                Database::Entity::DynamoDb::AttributeValue attributeValue;
                attributeValue.numberValue = request.attributes[k.attributeName].numberValue;
                attributeValue.numberSetValue = request.attributes[k.attributeName].numberSetValue;
                attributeValue.stringValue = request.attributes[k.attributeName].stringValue;
                attributeValue.numberSetValue = request.attributes[k.attributeName].numberSetValue;
                attributeValue.boolValue = request.attributes[k.attributeName].boolValue;
                attributeValue.nullValue = request.attributes[k.attributeName].nullValue;
                item.keys[k.attributeName] = attributeValue;
            }

            item = _dynamoDbDatabase.CreateOrUpdateItem(item);
            log_debug << "DynamoDb put item, region: " << item.region << " tableName: " << item.tableName;

            table.size += item.size;
            table.itemCount++;
            table = _dynamoDbDatabase.UpdateTable(table);
            log_debug << "Database updated, region: " << table.region << " tableName: " << table.name;

            Dto::DynamoDb::PutItemResponse response;
            response.consumedCapacity.tableName = item.tableName;
            response.item.region = item.region;
            response.item.tableName = item.tableName;
            response.item.attributes = Dto::DynamoDb::Mapper::map(item.attributes);
            response.item.keys = Dto::DynamoDb::Mapper::map(item.keys);
            response.item.created = item.created;
            response.item.modified = item.modified;
            return response;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb put item failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb put item failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::QueryResponse DynamoDbService::Query(const Dto::DynamoDb::QueryRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "query");
        log_debug << "Start query, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        try {

            // Send request to docker container
            std::map<std::string, std::string> headers = PrepareHeaders("Query");
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.ToJson(), headers);
            Dto::DynamoDb::QueryResponse queryResponse = Dto::DynamoDb::QueryResponse::FromJson(body);
            log_debug << "DynamoDb query item, name: " << request.tableName;

            return queryResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb query failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb query failed, error: " + exc.message());
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
            scanResponse.scannedCount = items.size();
            for (const auto &item: items) {
                scanResponse.items.emplace_back(Dto::DynamoDb::Mapper::map(item.attributes));
            }
            return scanResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb scan failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }
        return {};
    }

    Dto::DynamoDb::DeleteItemResponse DynamoDbService::DeleteItem(const Dto::DynamoDb::DeleteItemRequest &request) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_SERVICE_TIMER, DYNAMODB_SERVICE_COUNTER, "action", "delete_item");
        log_debug << "Start creating a new DynamoDb item, region: " << request.region << " table: " << request.tableName;

        if (Database::Entity::DynamoDb::Item item = Dto::DynamoDb::Mapper::map(request); !_dynamoDbDatabase.ItemExists(item)) {
            log_warning << "DynamoDb item does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        try {

            // Send request to docker container
            std::string tp = request.ToJson();
            std::map<std::string, std::string> headers = PrepareHeaders("DeleteItem");
            if (auto [body, oldHeaders, status] = SendAuthorizedDynamoDbRequest(request.ToJson(), headers); status == http::status::ok) {

                Dto::DynamoDb::DeleteItemResponse deleteItemResponse = Dto::DynamoDb::DeleteItemResponse::FromJson(body);

                // Delete item in database
                std::map<std::string, Database::Entity::DynamoDb::AttributeValue> keys;
                for (const auto &[fst, snd]: request.keys) {
                    Database::Entity::DynamoDb::AttributeValue attributeEntity;
                    if (!snd.stringValue.empty()) {
                        attributeEntity.stringValue = snd.stringValue;
                    } else if (!snd.numberValue.empty()) {
                        attributeEntity.numberValue = snd.numberValue;
                    } else if (snd.boolValue) {
                        attributeEntity.boolValue = snd.boolValue;
                    } else if (snd.nullValue && *snd.nullValue) {
                        attributeEntity.nullValue = snd.nullValue;
                    }
                    keys[fst] = attributeEntity;
                }
                _dynamoDbDatabase.DeleteItem(request.region, request.tableName, keys);

                return deleteItemResponse;
            }
            log_debug << "DynamoDb item deleted, table: " << request.tableName;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete item failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete item failed, message: " + exc.message());
        }
        return {};
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

    std::map<std::string, std::string> DynamoDbService::PrepareHeaders(const std::string &command) {
        std::map<std::string, std::string> headers;
        headers["Region"] = "eu-central-1";
        headers["User-Agent"] = "aws-cli/2.15.23 Python/3.11.6 Linux/6.1.0-18-amd64 exe/x86_64.debian.12 prompt/off command/dynamodb." + Core::StringUtils::ToSnakeCase(command);
        headers["Content-Type"] = "application/x-amz-json-1.0";
        headers["X-Amz-Target"] = "DynamoDB_20120810." + command;
        headers["X-Amz-Date"] = Core::DateTimeUtils::NowISO8601();
        headers["X-Amz-Security-Token"] = "none";
        return headers;
    }

    std::map<std::string, std::string> DynamoDbService::PrepareStreamHeaders(const std::string &command) {
        std::map<std::string, std::string> headers;
        headers["Region"] = "eu-central-1";
        headers["User-Agent"] = "aws-cli/2.25.1 md/awscrt#0.23.8 ua/2.1 os/linux#6.14.0-24-generic md/arch#x86_64 lang/python#3.12.9 md/pyimpl#CPython m/N cfg/retry-mode#standard md/installer#exe md/distrib#ubuntu.24 md/prompt#off md/command#dynamodbstreams." + Core::StringUtils::ToSnakeCase(command);
        headers["Content-Type"] = "application/x-amz-json-1.0";
        headers["X-Amz-Target"] = "DynamoDBStreams_20120810." + command;
        headers["X-Amz-Date"] = Core::DateTimeUtils::NowISO8601();
        headers["X-Amz-Security-Token"] = "none";
        return headers;
    }

    Dto::DynamoDb::DynamoDbResponse DynamoDbService::SendDynamoDbRequest(const std::string &body, const std::map<std::string, std::string> &headers) const {
        log_debug << "Sending DynamoDB container request, endpoint: " << _containerHost << ":" << _containerPort;

        auto [statusCode, outBody, outHeaders] = Core::HttpSocket::SendJson(http::verb::post, _containerHost, _containerPort, "/", body, headers);
        if (statusCode != http::status::ok) {
            log_error << "HTTP error, status: " << statusCode << " body: " << outBody;
            throw Core::ServiceException("HTTP error, status: " + boost::lexical_cast<std::string>(statusCode) + " reason: " + outBody);
        }
        return {.body = outBody, .headers = outHeaders, .status = statusCode};
    }

    Dto::DynamoDb::DynamoDbResponse DynamoDbService::SendAuthorizedDynamoDbRequest(const std::string &body, std::map<std::string, std::string> &headers) const {
        log_debug << "Sending DynamoDB container request, endpoint: " << _containerHost << ":" << _containerPort << ", body: " << body;

        const Core::HttpSocketResponse response = Core::HttpSocket::SendAuthorizedJson(http::verb::post, "dynamodb", _containerHost, _containerPort, "/", "content-type;host;x-amz-date;x-amz-security-token;x-amz-target", headers, body);
        if (response.statusCode != http::status::ok) {
            log_error << "HTTP error, status: " << response.statusCode << ", responseBody: " << response.body << ", requestBody: " << body << ", host: " << _containerHost << ":" << _containerPort;
            throw Core::ServiceException("HTTP error, status: " + boost::lexical_cast<std::string>(response.statusCode) + " reason: " + response.body);
        }
        log_debug << "SendAuthorizedDynamoDbRequest, success, returnBody: " << response.body;
        return {.body = response.body, .headers = headers, .status = response.statusCode};
    }

}// namespace AwsMock::Service
