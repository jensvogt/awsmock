//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_DYNAMODB_SERVICE_H
#define AWSMOCK_SERVICE_DYNAMODB_SERVICE_H

// C++ standard includes
#include <sstream>
#include <string>

// Boost include
#include <boost/lexical_cast.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/PagingUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/common/mapper/Mapper.h>
#include <awsmock/dto/dynamodb/CreateTableRequest.h>
#include <awsmock/dto/dynamodb/CreateTableResponse.h>
#include <awsmock/dto/dynamodb/DeleteAllItemsRequest.h>
#include <awsmock/dto/dynamodb/DeleteItemRequest.h>
#include <awsmock/dto/dynamodb/DeleteItemResponse.h>
#include <awsmock/dto/dynamodb/DeleteTableRequest.h>
#include <awsmock/dto/dynamodb/DeleteTableResponse.h>
#include <awsmock/dto/dynamodb/DescribeTableRequest.h>
#include <awsmock/dto/dynamodb/DescribeTableResponse.h>
#include <awsmock/dto/dynamodb/DynamoDbResponse.h>
#include <awsmock/dto/dynamodb/GetItemRequest.h>
#include <awsmock/dto/dynamodb/GetItemResponse.h>
#include <awsmock/dto/dynamodb/ListStreamsRequest.h>
#include <awsmock/dto/dynamodb/ListStreamsResponse.h>
#include <awsmock/dto/dynamodb/ListTableRequest.h>
#include <awsmock/dto/dynamodb/ListTableResponse.h>
#include <awsmock/dto/dynamodb/PutItemRequest.h>
#include <awsmock/dto/dynamodb/PutItemResponse.h>
#include <awsmock/dto/dynamodb/QueryRequest.h>
#include <awsmock/dto/dynamodb/QueryResponse.h>
#include <awsmock/dto/dynamodb/ScanRequest.h>
#include <awsmock/dto/dynamodb/ScanResponse.h>
#include <awsmock/dto/dynamodb/internal/GetTableDetailCountersRequest.h>
#include <awsmock/dto/dynamodb/internal/GetTableDetailCountersResponse.h>
#include <awsmock/dto/dynamodb/internal/ListItemCountersRequest.h>
#include <awsmock/dto/dynamodb/internal/ListItemCountersResponse.h>
#include <awsmock/dto/dynamodb/internal/ListTableArnsResponse.h>
#include <awsmock/dto/dynamodb/internal/ListTableCountersRequest.h>
#include <awsmock/dto/dynamodb/internal/ListTableCountersResponse.h>
#include <awsmock/dto/dynamodb/mapper/Mapper.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Service {

    /**
     * @brief DynamoDb module service.
     *
     * @par
     * Handles all DynamoDb related requests. The DynamoDB itself runs as docker container on the local host. It will start the DynamoDB server automatically as soon
     * as the server starts. DynamoDB commands will be sent via HTTP to the DynamoDB docker container on port 8000. The docker port can be configured in the AwsMock
     * configuration properties.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbService {

      public:

        /**
         * @brief Constructor
         */
        explicit DynamoDbService();

        /**
         * @brief Creates a new table
         *
         * @param request create table request DTO
         * @return CreateTableResponse
         */
        [[nodiscard]] Dto::DynamoDb::CreateTableResponse CreateTable(const Dto::DynamoDb::CreateTableRequest &request) const;

        /**
         * @brief check existence of table in DynamoDB docker image.
         *
         * @param region AWS region
         * @param tableName table name
         * @return true, if the table exists in dynamoDb docker image
         */
        [[nodiscard]] bool ExistTable(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Lists all available tables
         *
         * @param request list table request DTO
         * @return ListTableResponse
         */
        [[nodiscard]] Dto::DynamoDb::ListTableResponse ListTables(const Dto::DynamoDb::ListTableRequest &request) const;

        /**
         * @brief Lists all table counters
         *
         * @param request list table counters request DTO
         * @return ListTableCountersResponse
         * @see ListTableCountersResponse
         */
        [[nodiscard]] Dto::DynamoDb::ListTableCountersResponse ListTableCounters(const Dto::DynamoDb::ListTableCountersRequest &request) const;

        /**
         * @brief Get  table detail counters
         *
         * @param request get table detail counters request DTO
         * @return GetTableDetailCountersResponse
         * @see GetTableDetailCountersRequest
         * @see GetTableDetailCountersResponse
         */
        [[nodiscard]] Dto::DynamoDb::GetTableDetailCountersResponse GetTableDetailCounters(const Dto::DynamoDb::GetTableDetailCountersRequest &request) const;

        /**
         * @brief Lists all table counters
         *
         * @param region AWS region
         * @return Dto::DynamoDb::ListTableArnsResponse
         * @see Dto::DynamoDb::ListTableArnsResponse
         */
        [[nodiscard]] Dto::DynamoDb::ListTableArnsResponse ListTableArns(const std::string &region) const;

        /**
         * @brief Lists all table counters
         *
         * @param request list streams request
         * @return Dto::DynamoDb::ListStreamsResponse
         * @see Dto::DynamoDb::ListStreamsResponse
         */
        [[nodiscard]] Dto::DynamoDb::ListStreamsResponse ListStreams(const Dto::DynamoDb::ListStreamsRequest &request) const;

        /**
         * @brief Lists all item counters
         *
         * @param request list item counters request DTO
         * @return ListItemCountersResponse
         * @see ListItemCountersResponse
         */
        [[nodiscard]] Dto::DynamoDb::ListItemCountersResponse ListItemCounters(const Dto::DynamoDb::ListItemCountersRequest &request) const;

        /**
         * @brief Describes a table
         *
         * @param request describe table request DTO
         * @return DescribeTableResponse
         */
        [[nodiscard]] Dto::DynamoDb::DescribeTableResponse DescribeTable(const Dto::DynamoDb::DescribeTableRequest &request) const;

        /**
         * @brief Deletes a table
         *
         * @param request delete table request DTO
         * @return DeleteTableResponse
         */
        [[nodiscard]] Dto::DynamoDb::DeleteTableResponse DeleteTable(const Dto::DynamoDb::DeleteTableRequest &request) const;

        /**
         * @brief Deletes all tables with all items
         */
        [[nodiscard]] long DeleteAllTables() const;

        /**
         * Gets an item
         *
         * @param request get item request DTO
         * @return GetItemResponse
         */
        [[nodiscard]] Dto::DynamoDb::GetItemResponse GetItem(const Dto::DynamoDb::GetItemRequest &request) const;

        /**
         * @brief Puts an item
         *
         * @param request put item request DTO
         * @return GetItemResponse
         */
        [[nodiscard]] Dto::DynamoDb::PutItemResponse PutItem(const Dto::DynamoDb::PutItemRequest &request) const;

        /**
         * @brief Query the database
         *
         * @param request query item request DTO
         * @return QueryResponse
         */
        [[nodiscard]] Dto::DynamoDb::QueryResponse Query(const Dto::DynamoDb::QueryRequest &request) const;

        /**
         * @brief Scan the database
         *
         * @param request scan request DTO
         * @return ScanResponse
         */
        [[nodiscard]] Dto::DynamoDb::ScanResponse Scan(const Dto::DynamoDb::ScanRequest &request) const;

        /**
         * @brief Deletes a item
         *
         * @param request delete item request DTO
         * @return DeleteItemResponse
         */
        [[nodiscard]] Dto::DynamoDb::DeleteItemResponse DeleteItem(const Dto::DynamoDb::DeleteItemRequest &request) const;

        /**
         * @brief delete all items
         *
         * @par
         * As DynamoDB does not have a truncate, or purge the table is deleted and recreated.
         *
         * @param request delete items request
         */
        void DeleteAllItems(const Dto::DynamoDb::DeleteAllItemsRequest &request) const;

      private:

        /**
         * @brief Send the request to the DynamoDB container.
         *
         * @par
         * The authorization header is taken from the original request.
         *
         * @param body original HTTP request body
         * @param headers original HTTP request headers
         * @return response body
         */
        [[nodiscard]] Dto::DynamoDb::DynamoDbResponse SendDynamoDbRequest(const std::string &body, const std::map<std::string, std::string> &headers) const;

        /**
         * @brief Send the request to the DynamoDB container.
         *
         * @par
         * The authorization header is calculated from the request parameter.
         *
         * @param body original HTTP request body
         * @param headers original HTTP request headers
         * @return response body
         */
        Dto::DynamoDb::DynamoDbResponse SendAuthorizedDynamoDbRequest(const std::string &body, std::map<std::string, std::string> &headers) const;

        /**
         * @brief Prepare the headers for the docker request
         *
         * @param command DynamoDB command
         */
        static std::map<std::string, std::string> PrepareHeaders(const std::string &command);

        /**
         * @brief Prepare the headers for the docker request
         *
         * @param command DynamoDB streams command
         */
        static std::map<std::string, std::string> PrepareStreamHeaders(const std::string &command);

        /**
         * Database connection
         */
        Database::DynamoDbDatabase &_dynamoDbDatabase;

        /**
         * DynamoDb container host
         */
        std::string _containerHost;

        /**
         * DynamoDb container port
         */
        int _containerPort;

        /**
         * AWS account ID
         */
        std::string _accountId;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_DYNAMODB_SERVICE_H
