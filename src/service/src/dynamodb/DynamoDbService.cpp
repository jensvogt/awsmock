//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/dynamodb/DynamoDbService.h>

namespace AwsMock::Service {

    DynamoDbService::DynamoDbService() : _dynamoDbDatabase(Database::DynamoDbDatabase::instance()) {

        // DynamoDB docker host, port
        _containerHost = Core::Configuration::instance().GetValueString("awsmock.modules.dynamodb.container.host");
        _containerPort = Core::Configuration::instance().GetValueInt("awsmock.modules.dynamodb.container.port");
    }

    Dto::DynamoDb::CreateTableResponse DynamoDbService::CreateTable(const Dto::DynamoDb::CreateTableRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "create_table");
        log_debug << "Start creating a new DynamoDb table, region: " << request.region << " name: " << request.tableName;

        if (_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table exists already, region: " << request.region << " name: " << request.tableName;
            return {};
        }

        Dto::DynamoDb::CreateTableResponse createTableResponse;
        try {

            // Send request to DynamoDB docker container
            std::map<std::string, std::string> headers = request.headers;
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers);
            createTableResponse = {.body = body, .headers = outHeaders, .status = status};
            createTableResponse.ScanResponse();

            // Update database
            Database::Entity::DynamoDb::Table table = {
                    .region = request.region,
                    .name = request.tableName,
                    .attributes = request.attributes,
                    .tags = request.tags,
                    .keySchemas = request.keySchemas};
            table = _dynamoDbDatabase.CreateTable(table);
            log_info << "DynamoDb table created, name: " << table.name;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd create table failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd create table failed, error: " + exc.message());
        }

        return createTableResponse;
    }

    Dto::DynamoDb::ListTableResponse DynamoDbService::ListTables(const Dto::DynamoDb::ListTableRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "list_tables");
        log_debug << "Starting list table request, region: " << request.region;

        try {

            // Send request to docker container
            std::map<std::string, std::string> headers = request.headers;
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers);
            Dto::DynamoDb::ListTableResponse listTableResponse = {.body = body, .headers = outHeaders, .status = status};
            listTableResponse.ScanResponse();
            log_trace << "DynamoDb list tables, region: " << request.region << " body: " << body;
            return listTableResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list tables failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd list tables failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::ListTableCountersResponse DynamoDbService::ListTableCounters(const Dto::DynamoDb::ListTableCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "list_table_counters");
        log_debug << "Starting list table request, region: " << request.region;

        try {

            Dto::DynamoDb::ListTableCountersResponse tableResponse;
            tableResponse.total = _dynamoDbDatabase.CountTables(request.region, request.prefix);
            for (std::vector<Database::Entity::DynamoDb::Table> tables = _dynamoDbDatabase.ListTables(request.region); const auto &table: tables) {
                Dto::DynamoDb::TableCounter tableCounter;
                tableCounter.tableName = table.name;
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

    Dto::DynamoDb::ListItemCountersResponse DynamoDbService::ListItemCounters(const Dto::DynamoDb::ListItemCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "list_item_counters");
        log_debug << "Starting list item request, region: " << request.region;

        try {
            Dto::DynamoDb::ListItemCountersResponse itemResponse;
            itemResponse.total = _dynamoDbDatabase.CountItems(request.region, request.tableName, request.prefix);
            itemResponse.itemCounters = _dynamoDbDatabase.ListItems(request.region, request.tableName);
            return itemResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd list item counters failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd list item counters failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::DescribeTableResponse DynamoDbService::DescribeTable(const Dto::DynamoDb::DescribeTableRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "describe_table");
        log_debug << "Describe DynamoDb table, region: " << request.region << " name: " << request.tableName;

        Dto::DynamoDb::DescribeTableResponse describeTableResponse;
        describeTableResponse.region = request.region;

        try {
            // Send request to docker container
            std::map<std::string, std::string> headers = request.headers;
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers);
            describeTableResponse = {.body = body, .headers = outHeaders, .status = status};
            describeTableResponse.PrepareResponse();
            log_debug << "DynamoDb describe table, name: " << request.tableName;
            return describeTableResponse;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb describe table failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb describe table failed, error: " + exc.message());
        }
    }

    Dto::DynamoDb::DeleteTableResponse DynamoDbService::DeleteTable(const Dto::DynamoDb::DeleteTableRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "delete_table");
        log_debug << "Start creating a new DynamoDb table, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table does not exist, region: " + request.region + " name: " + request.tableName);
        }

        Dto::DynamoDb::DeleteTableResponse deleteTableResponse;
        try {

            // Send request to DynamoDB docker container
            std::map<std::string, std::string> headers = PrepareHeaders("DeleteTable");
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers);
            deleteTableResponse = {.body = body, .headers = outHeaders, .status = status};

            // Delete table in database
            _dynamoDbDatabase.DeleteTable(request.region, request.tableName);
            log_info << "DynamoDb table deleted, name: " << request.tableName;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete table failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete table failed, error: " + exc.message());
        }

        return deleteTableResponse;
    }

    void DynamoDbService::DeleteAllTables() const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "delete_all_tables");
        log_debug << "Deleting all tables";

        try {

            // Delete all tables from DynamoDB
            for (const auto &table: _dynamoDbDatabase.ListTables()) {

                Dto::DynamoDb::DeleteTableRequest dynamoDeleteRequest;
                dynamoDeleteRequest.tableName = table.name;
                dynamoDeleteRequest.body = dynamoDeleteRequest.ToJson();
                dynamoDeleteRequest.headers = PrepareHeaders("DeleteTable");
                SendAuthorizedDynamoDbRequest(dynamoDeleteRequest.body, dynamoDeleteRequest.headers);
            }

            // Delete table in database
            _dynamoDbDatabase.DeleteAllTables();
            log_info << "DynamoDb tables deleted";

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete table failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete table failed, message: " + exc.message());
        }
    }

    Dto::DynamoDb::GetItemResponse DynamoDbService::GetItem(const Dto::DynamoDb::GetItemRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "get_item");
        log_debug << "Start get item, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        Dto::DynamoDb::GetItemResponse getItemResponse;
        try {

            // Send request to docker container
            std::map<std::string, std::string> headers = PrepareHeaders("GetItem");
            if (auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers); status == http::status::ok) {

                getItemResponse = {.body = body, .headers = outHeaders, .status = status};
                log_info << "DynamoDb get item, name: " << request.tableName;
            }
        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd get item failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDbd get item failed, , error: " + exc.message());
        }

        return getItemResponse;
    }

    Dto::DynamoDb::PutItemResponse DynamoDbService::PutItem(const Dto::DynamoDb::PutItemRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "put_item");
        log_debug << "Start put item, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        Dto::DynamoDb::PutItemResponse putItemResponse;
        try {

            // Send request to docker container
            std::map<std::string, std::string> headers = PrepareHeaders("PutItem");
            if (auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers); status == http::status::ok) {

                putItemResponse = {.body = body, .headers = outHeaders, .status = status};

                // Get the table
                Database::Entity::DynamoDb::Table table = _dynamoDbDatabase.GetTableByRegionName(request.region, request.tableName);

                // Convert to entity and save to database. If no exception is thrown by the HTTP call to the docker image, seems to be ok.
                Database::Entity::DynamoDb::Item item = Dto::DynamoDb::Mapper::map(request, table);
                item = _dynamoDbDatabase.CreateOrUpdateItem(item);
                log_info << "DynamoDb put item, region: " << item.region << " tableName: " << item.tableName;
            }

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb put item failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb put item failed, error: " + exc.message());
        }

        return putItemResponse;
    }

    Dto::DynamoDb::QueryResponse DynamoDbService::Query(const Dto::DynamoDb::QueryRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "query");
        log_debug << "Start query, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        Dto::DynamoDb::QueryResponse queryResponse;
        try {

            // Send request to docker container
            std::map<std::string, std::string> headers = request.headers;
            auto [body, outHeaders, status] = SendDynamoDbRequest(request.body, headers);
            queryResponse = {.body = body, .headers = outHeaders, .status = status};
            log_info << "DynamoDb query item, name: " << request.tableName;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb query failed, error: " << exc.message();
            throw Core::ServiceException("DynamoDb query failed, error: " + exc.message());
        }

        return queryResponse;
    }

    Dto::DynamoDb::ScanResponse DynamoDbService::Scan(const Dto::DynamoDb::ScanRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "scan");
        log_debug << "Start scan, region: " << request.region << " name: " << request.tableName;

        if (!_dynamoDbDatabase.TableExists(request.region, request.tableName)) {
            log_warning << "DynamoDb table does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        Dto::DynamoDb::ScanResponse scanResponse;
        try {

            // Send request to docker container
            std::map<std::string, std::string> headers = PrepareHeaders("Scan");
            auto [body, outHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers);
            scanResponse = {.body = body, .headers = outHeaders, .status = status};
            log_info << "DynamoDb scan item, name: " << request.tableName;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDb scan failed, message: " << exc.message();
            throw Core::ServiceException(exc.message());
        }

        return scanResponse;
    }

    Dto::DynamoDb::DeleteItemResponse DynamoDbService::DeleteItem(const Dto::DynamoDb::DeleteItemRequest &request) const {
        Monitoring::MetricServiceTimer measure(DYNAMODB_SERVICE_TIMER, "method", "delete_item");
        log_debug << "Start creating a new DynamoDb item, region: " << request.region << " table: " << request.tableName;

        if (Database::Entity::DynamoDb::Item item = Dto::DynamoDb::Mapper::map(request); !_dynamoDbDatabase.ItemExists(item)) {
            log_warning << "DynamoDb item does not exist, region: " << request.region << " name: " << request.tableName;
            throw Core::BadRequestException("DynamoDb table exists already, region: " + request.region + " name: " + request.tableName);
        }

        Dto::DynamoDb::DeleteItemResponse deleteItemResponse;
        try {

            // Send request to docker container
            std::map<std::string, std::string> headers = PrepareHeaders("DeleteItem");
            auto [body, oldHeaders, status] = SendAuthorizedDynamoDbRequest(request.body, headers);
            deleteItemResponse = {.body = body, .headers = oldHeaders, .status = status};

            if (status == http::status::ok) {

                // Delete item in database
                _dynamoDbDatabase.DeleteItem(request.region, request.tableName, "");
            }
            log_info << "DynamoDb item deleted, table: " << request.tableName;

        } catch (Core::JsonException &exc) {
            log_error << "DynamoDbd delete item failed, message: " << exc.message();
            throw Core::ServiceException("DynamoDbd delete item failed, message: " + exc.message());
        }

        return deleteItemResponse;
    }

    std::map<std::string, std::string> DynamoDbService::PrepareHeaders(const std::string &command) const {
        std::map<std::string, std::string> headers;
        const std::string commandSnake = Core::StringUtils::ToSnakeCase(command);
        headers["Region"] = "eu-central-1";
        headers["User-Agent"] = "aws-cli/2.15.23 Python/3.11.6 Linux/6.1.0-18-amd64 exe/x86_64.debian.12 prompt/off command/dynamodb." + commandSnake;
        headers["Content-Type"] = "application/x-amz-json-1.0";
        headers["X-Amz-Target"] = "DynamoDB_20120810." + command;
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
        log_debug << "Sending DynamoDB container request, endpoint: " << _containerHost << ":" << _containerPort;

        const Core::HttpSocketResponse response = Core::HttpSocket::SendAuthorizedJson(http::verb::post, "dynamodb", _containerHost, _containerPort, "/", "content-type;host;x-amz-date;x-amz-security-token;x-amz-target", headers, body);
        if (response.statusCode != http::status::ok) {
            log_error << "HTTP error, status: " << response.statusCode << " body: " << response.body;
            throw Core::ServiceException("HTTP error, status: " + boost::lexical_cast<std::string>(response.statusCode) + " reason: " + response.body);
        }
        return {.body = response.body, .headers = headers, .status = response.statusCode};
    }

}// namespace AwsMock::Service
