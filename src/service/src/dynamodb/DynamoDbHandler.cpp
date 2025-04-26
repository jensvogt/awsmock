
#include "awsmock/core/exception/UnauthorizedException.h"
#include <awsmock/service/dynamodb/DynamoDbHandler.h>

namespace AwsMock::Service {

    http::response<http::dynamic_body> DynamoDbHandler::HandlePostRequest(const http::request<http::dynamic_body> &request, const std::string &region, const std::string &user) {
        log_trace << "DynamoDb POST request, URI: " << request.target() << " region: " << region << " user: " << user;

        Dto::Common::DynamoDbClientCommand clientCommand;
        clientCommand.FromRequest(request, region, user);

        try {

            switch (clientCommand.command) {

                case Dto::Common::DynamoDbCommandType::CREATE_TABLE: {

                    Dto::DynamoDb::CreateTableRequest tableRequest;
                    tableRequest.FromJson(clientCommand.payload);
                    tableRequest.region = clientCommand.region;
                    tableRequest.requestId = clientCommand.requestId;
                    tableRequest.user = clientCommand.user;
                    tableRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::CreateTableResponse tableResponse = _dynamoDbService.CreateTable(tableRequest);
                    if (tableResponse.status == http::status::ok) {
                        return SendOkResponse(request, tableResponse.body, tableResponse.headers);
                    }
                    return SendInternalServerError(request, tableResponse.body, tableResponse.headers);
                    log_info << "Table created, name: " << tableRequest.tableName;
                }

                case Dto::Common::DynamoDbCommandType::LIST_TABLES: {

                    Dto::DynamoDb::ListTableRequest tableRequest;
                    tableRequest.FromJson(clientCommand.payload);
                    tableRequest.region = clientCommand.region;
                    tableRequest.requestId = clientCommand.requestId;
                    tableRequest.user = clientCommand.user;
                    tableRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::ListTableResponse tableResponse = _dynamoDbService.ListTables(tableRequest);
                    if (tableResponse.status == http::status::ok) {
                        return SendOkResponse(request, tableResponse.body, tableResponse.headers);
                    }
                    return SendInternalServerError(request, tableResponse.body, tableResponse.headers);
                    log_info << "Table listed, region: " << tableRequest.region;
                }

                case Dto::Common::DynamoDbCommandType::LIST_TABLE_COUNTERS: {

                    Dto::DynamoDb::ListTableCountersRequest tableRequest = Dto::DynamoDb::ListTableCountersRequest::FromJson(clientCommand.payload);
                    tableRequest.region = clientCommand.region;
                    tableRequest.requestId = clientCommand.requestId;
                    tableRequest.user = clientCommand.user;

                    Dto::DynamoDb::ListTableCountersResponse tableResponse = _dynamoDbService.ListTableCounters(tableRequest);
                    log_debug << "Table counters listed, region: " << tableRequest.region;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::LIST_ITEM_COUNTERS: {

                    Dto::DynamoDb::ListItemCountersRequest itemRequest = Dto::DynamoDb::ListItemCountersRequest::FromJson(clientCommand.payload);
                    itemRequest.region = clientCommand.region;
                    itemRequest.requestId = clientCommand.requestId;
                    itemRequest.user = clientCommand.user;

                    Dto::DynamoDb::ListItemCountersResponse itemResponse = _dynamoDbService.ListItemCounters(itemRequest);
                    log_debug << "Item counters listed, region: " << itemRequest.region << ", tableName: " << itemRequest.tableName;
                    return SendOkResponse(request, itemResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::DESCRIBE_TABLE: {

                    Dto::DynamoDb::DescribeTableRequest tableRequest;
                    tableRequest.FromJson(clientCommand.payload);
                    tableRequest.region = clientCommand.region;
                    tableRequest.requestId = clientCommand.requestId;
                    tableRequest.user = clientCommand.user;
                    tableRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::DescribeTableResponse tableResponse = _dynamoDbService.DescribeTable(tableRequest);
                    if (tableResponse.status == http::status::ok) {
                        return SendOkResponse(request, tableResponse.body, tableResponse.headers);
                    }
                    return SendInternalServerError(request, tableResponse.body, tableResponse.headers);
                }

                case Dto::Common::DynamoDbCommandType::DELETE_TABLE: {

                    Dto::DynamoDb::DeleteTableRequest tableRequest;
                    tableRequest.FromJson(clientCommand.payload);
                    tableRequest.region = clientCommand.region;
                    tableRequest.requestId = clientCommand.requestId;
                    tableRequest.user = clientCommand.user;
                    tableRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::DeleteTableResponse tableResponse = _dynamoDbService.DeleteTable(tableRequest);
                    if (tableResponse.status == http::status::ok) {
                        return SendOkResponse(request, tableResponse.body, tableResponse.headers);
                    }
                    return SendInternalServerError(request, tableResponse.body, tableResponse.headers);
                }

                case Dto::Common::DynamoDbCommandType::GET_ITEM: {

                    Dto::DynamoDb::GetItemRequest itemRequest;
                    itemRequest.FromJson(clientCommand.payload);
                    itemRequest.region = clientCommand.region;
                    itemRequest.requestId = clientCommand.requestId;
                    itemRequest.user = clientCommand.user;
                    itemRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::GetItemResponse itemResponse = _dynamoDbService.GetItem(itemRequest);
                    //itemResponse.headers["Content-Length"] = itemResponse.body.length();
                    if (itemResponse.status == http::status::ok) {
                        return SendOkResponse(request, itemResponse.body, itemResponse.headers);
                    }
                    return SendInternalServerError(request, itemResponse.body, itemResponse.headers);
                }

                case Dto::Common::DynamoDbCommandType::PUT_ITEM: {

                    Dto::DynamoDb::PutItemRequest itemRequest;
                    itemRequest.FromJson(clientCommand.payload);
                    itemRequest.region = clientCommand.region;
                    itemRequest.requestId = clientCommand.requestId;
                    itemRequest.user = clientCommand.user;
                    itemRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::PutItemResponse itemResponse = _dynamoDbService.PutItem(itemRequest);
                    if (itemResponse.status == http::status::ok) {
                        return SendOkResponse(request, itemResponse.body, itemResponse.headers);
                    }
                    return SendInternalServerError(request, itemResponse.body, itemResponse.headers);
                }

                case Dto::Common::DynamoDbCommandType::QUERY: {

                    Dto::DynamoDb::QueryRequest queryRequest;
                    queryRequest.FromJson(clientCommand.payload);
                    queryRequest.region = clientCommand.region;
                    queryRequest.requestId = clientCommand.requestId;
                    queryRequest.user = clientCommand.user;
                    queryRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);
                    if (queryResponse.status == http::status::ok) {
                        return SendOkResponse(request, queryResponse.body, queryResponse.headers);
                    }
                    return SendInternalServerError(request, queryResponse.body, queryResponse.headers);
                }

                case Dto::Common::DynamoDbCommandType::SCAN: {

                    Dto::DynamoDb::ScanRequest scanRequest;
                    scanRequest.FromJson(clientCommand.payload);
                    scanRequest.region = clientCommand.region;
                    scanRequest.requestId = clientCommand.requestId;
                    scanRequest.user = clientCommand.user;
                    scanRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::ScanResponse scanResponse = _dynamoDbService.Scan(scanRequest);
                    if (scanResponse.status == http::status::ok) {
                        return SendOkResponse(request, scanResponse.body, scanResponse.headers);
                    }
                    return SendInternalServerError(request, scanResponse.body, scanResponse.headers);
                }

                case Dto::Common::DynamoDbCommandType::DELETE_ITEM: {

                    Dto::DynamoDb::DeleteItemRequest dynamoDbRequest;
                    dynamoDbRequest.FromJson(clientCommand.payload);
                    dynamoDbRequest.region = clientCommand.region;
                    dynamoDbRequest.requestId = clientCommand.requestId;
                    dynamoDbRequest.user = clientCommand.user;
                    dynamoDbRequest.headers = clientCommand.headers;

                    Dto::DynamoDb::DeleteItemResponse dynamoDbResponse = _dynamoDbService.DeleteItem(dynamoDbRequest);
                    if (dynamoDbResponse.status == http::status::ok) {
                        return SendOkResponse(request, dynamoDbResponse.body, dynamoDbResponse.headers);
                    }
                    return SendInternalServerError(request, dynamoDbResponse.body, dynamoDbResponse.headers);
                }

                case Dto::Common::DynamoDbCommandType::UNKNOWN: {
                    log_error << "Bad request, method: POST clientCommand: " << Dto::Common::DynamoDbCommandTypeToString(clientCommand.command);
                    return SendBadRequestError(request, "Bad request, method: POST clientCommand: " + Dto::Common::DynamoDbCommandTypeToString(clientCommand.command));
                }

                default: {
                    log_error << "Bad request, method: POST clientCommand: " << Dto::Common::DynamoDbCommandTypeToString(clientCommand.command);
                    return SendBadRequestError(request);
                }
            }

        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (...) {
            return SendInternalServerError(request, "Unknown exception");
        }
    }

}// namespace AwsMock::Service
