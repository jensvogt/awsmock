
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

                    Dto::DynamoDb::CreateTableRequest tableRequest = Dto::DynamoDb::CreateTableRequest::FromJson(clientCommand);
                    Dto::DynamoDb::CreateTableResponse tableResponse = _dynamoDbService.CreateTable(tableRequest);
                    log_info << "Table created, name: " << tableRequest.tableName;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::LIST_TABLES: {

                    Dto::DynamoDb::ListTableRequest tableRequest = Dto::DynamoDb::ListTableRequest::FromJson(clientCommand);
                    Dto::DynamoDb::ListTableResponse tableResponse = _dynamoDbService.ListTables(tableRequest);
                    log_info << "Table listed, region: " << tableRequest.region;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::LIST_TABLE_COUNTERS: {

                    Dto::DynamoDb::ListTableCountersRequest tableRequest = Dto::DynamoDb::ListTableCountersRequest::FromJson(clientCommand.payload);
                    Dto::DynamoDb::ListTableCountersResponse tableResponse = _dynamoDbService.ListTableCounters(tableRequest);
                    log_debug << "List table counters listed, region: " << tableRequest.region << ", count: " << tableResponse.total;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::GET_TABLE_DETAIL_COUNTERS: {

                    Dto::DynamoDb::GetTableDetailCountersRequest tableRequest = Dto::DynamoDb::GetTableDetailCountersRequest::FromJson(clientCommand.payload);
                    Dto::DynamoDb::GetTableDetailCountersResponse tableResponse = _dynamoDbService.GetTableDetailCounters(tableRequest);
                    log_debug << "Get table details counters, region: " << tableRequest.region << ", tableName: " << tableRequest.tableName;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::LIST_TABLE_ARNS: {

                    Dto::DynamoDb::ListTableArnsResponse tableResponse = _dynamoDbService.ListTableArns(region);
                    log_debug << "List table arns, region: " << region;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::LIST_ITEM_COUNTERS: {

                    Dto::DynamoDb::ListItemCountersRequest itemRequest = Dto::DynamoDb::ListItemCountersRequest::FromJson(clientCommand.payload);
                    Dto::DynamoDb::ListItemCountersResponse itemResponse = _dynamoDbService.ListItemCounters(itemRequest);
                    log_debug << "Item counters listed, region: " << itemRequest.region << ", tableName: " << itemRequest.tableName << ", count: " << itemResponse.total;
                    return SendOkResponse(request, itemResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::LIST_STREAMS: {

                    Dto::DynamoDb::ListStreamsRequest tableRequest = Dto::DynamoDb::ListStreamsRequest::FromJson(clientCommand);
                    Dto::DynamoDb::ListStreamsResponse tableResponse = _dynamoDbService.ListStreams(tableRequest);
                    log_info << "Streams listed, region: " << tableRequest.region << ", tableName: " << tableRequest.tableName;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::DESCRIBE_TABLE: {

                    Dto::DynamoDb::DescribeTableRequest tableRequest = Dto::DynamoDb::DescribeTableRequest::FromJson(clientCommand);
                    Dto::DynamoDb::DescribeTableResponse tableResponse = _dynamoDbService.DescribeTable(tableRequest);
                    log_debug << "Describe table, region: " << tableRequest.region << ", tableName: " << tableRequest.tableName;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::DELETE_TABLE: {

                    Dto::DynamoDb::DeleteTableRequest tableRequest = Dto::DynamoDb::DeleteTableRequest::FromJson(clientCommand);
                    Dto::DynamoDb::DeleteTableResponse tableResponse = _dynamoDbService.DeleteTable(tableRequest);
                    log_debug << "Delete table, region: " << tableRequest.region << ", tableName: " << tableRequest.tableName;
                    return SendOkResponse(request, tableResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::GET_ITEM: {

                    Dto::DynamoDb::GetItemRequest itemRequest = Dto::DynamoDb::GetItemRequest::FromJson(clientCommand);
                    Dto::DynamoDb::GetItemResponse itemResponse = _dynamoDbService.GetItem(itemRequest);
                    log_debug << "Get item, region: " << itemRequest.region << ", tableName: " << itemRequest.tableName;
                    return SendOkResponse(request, itemResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::PUT_ITEM: {

                    Dto::DynamoDb::PutItemRequest itemRequest = Dto::DynamoDb::PutItemRequest::FromJson(clientCommand);
                    Dto::DynamoDb::PutItemResponse itemResponse = _dynamoDbService.PutItem(itemRequest);
                    log_debug << "Put item, region: " << itemRequest.region << ", tableName: " << itemRequest.tableName;
                    return SendOkResponse(request, itemResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::QUERY: {

                    Dto::DynamoDb::QueryRequest queryRequest = Dto::DynamoDb::QueryRequest::FromJson(clientCommand);
                    Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);
                    log_debug << "Query, region: " << queryRequest.region << ", tableName: " << queryRequest.tableName << ", response: " << queryResponse.ToJson();
                    return SendOkResponse(request, queryResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::SCAN: {

                    Dto::DynamoDb::ScanRequest scanRequest = Dto::DynamoDb::ScanRequest::FromJson(clientCommand);
                    Dto::DynamoDb::ScanResponse scanResponse = _dynamoDbService.Scan(scanRequest);
                    log_debug << "Scan, region: " << scanRequest.region << ", tableName: " << scanRequest.tableName << ", response: " << scanResponse.ToJson();
                    return SendOkResponse(request, scanResponse.ToJson());
                }

                case Dto::Common::DynamoDbCommandType::DELETE_ITEM: {

                    Dto::DynamoDb::DeleteItemRequest dynamoDbRequest = Dto::DynamoDb::DeleteItemRequest::FromJson(clientCommand);
                    Dto::DynamoDb::DeleteItemResponse dynamoDbResponse = _dynamoDbService.DeleteItem(dynamoDbRequest);
                    return SendOkResponse(request, dynamoDbResponse.ToJson());
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

        } catch (Core::BadRequestException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::ServiceException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::JsonException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (Core::DatabaseException &exc) {
            log_error << exc.message();
            return SendInternalServerError(request, exc.message());
        } catch (boost::exception &exc) {
            log_error << diagnostic_information(exc);
            return SendInternalServerError(request, "Unknown exception");
        }
    }

}// namespace AwsMock::Service
