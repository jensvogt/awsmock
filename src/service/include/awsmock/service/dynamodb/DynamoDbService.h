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
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/SystemUtils.h>
#include <awsmock/core/TarUtils.h>
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/dynamodb/CreateTableRequest.h>
#include <awsmock/dto/dynamodb/CreateTableResponse.h>
#include <awsmock/dto/dynamodb/DeleteItemRequest.h>
#include <awsmock/dto/dynamodb/DeleteItemResponse.h>
#include <awsmock/dto/dynamodb/DeleteTableRequest.h>
#include <awsmock/dto/dynamodb/DeleteTableResponse.h>
#include <awsmock/dto/dynamodb/DescribeTableRequest.h>
#include <awsmock/dto/dynamodb/DescribeTableResponse.h>
#include <awsmock/dto/dynamodb/DynamoDbResponse.h>
#include <awsmock/dto/dynamodb/GetItemRequest.h>
#include <awsmock/dto/dynamodb/GetItemResponse.h>
#include <awsmock/dto/dynamodb/ListItemCountersRequest.h>
#include <awsmock/dto/dynamodb/ListItemCountersResponse.h>
#include <awsmock/dto/dynamodb/ListTableCountersRequest.h>
#include <awsmock/dto/dynamodb/ListTableCountersResponse.h>
#include <awsmock/dto/dynamodb/ListTableRequest.h>
#include <awsmock/dto/dynamodb/ListTableResponse.h>
#include <awsmock/dto/dynamodb/PutItemRequest.h>
#include <awsmock/dto/dynamodb/PutItemResponse.h>
#include <awsmock/dto/dynamodb/QueryRequest.h>
#include <awsmock/dto/dynamodb/QueryResponse.h>
#include <awsmock/dto/dynamodb/ScanRequest.h>
#include <awsmock/dto/dynamodb/ScanResponse.h>
#include <awsmock/dto/dynamodb/mapper/Mapper.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/service/container/ContainerService.h>
#include <awsmock/service/monitoring/MetricDefinition.h>
#include <awsmock/service/monitoring/MetricService.h>
#include <awsmock/service/monitoring/MetricServiceTimer.h>

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
        Dto::DynamoDb::CreateTableResponse CreateTable(const Dto::DynamoDb::CreateTableRequest &request) const;

        /**
         * @brief check existence of table in DynamoDB docker image.
         *
         * @param region AWS region
         * @param tableName table name
         * @return true if table exists in dynamoDb docker image
         */
        bool ExistTable(const std::string &region, const std::string &tableName) const;

        /**
         * @brief Lists all available tables
         *
         * @param request list table request DTO
         * @return ListTableResponse
         */
        Dto::DynamoDb::ListTableResponse ListTables(const Dto::DynamoDb::ListTableRequest &request) const;

        /**
         * @brief Lists all table counters
         *
         * @param request list table counters request DTO
         * @return ListTableCountersResponse
         * @see ListTableCountersResponse
         */
        Dto::DynamoDb::ListTableCountersResponse ListTableCounters(const Dto::DynamoDb::ListTableCountersRequest &request) const;

        /**
         * @brief Lists all item counters
         *
         * @param request list item counters request DTO
         * @return ListItemCountersResponse
         * @see ListItemCountersResponse
         */
        Dto::DynamoDb::ListItemCountersResponse ListItemCounters(const Dto::DynamoDb::ListItemCountersRequest &request) const;

        /**
         * @brief Describes a table
         *
         * @param request describe table request DTO
         * @return DescribeTableResponse
         */
        Dto::DynamoDb::DescribeTableResponse DescribeTable(const Dto::DynamoDb::DescribeTableRequest &request) const;

        /**
         * @brief Deletes a table
         *
         * @param request delete table request DTO
         * @return DeleteTableResponse
         */
        Dto::DynamoDb::DeleteTableResponse DeleteTable(const Dto::DynamoDb::DeleteTableRequest &request) const;

        /**
         * @brief Deletes all tables with all items
         */
        long DeleteAllTables() const;

        /**
         * Gets an item
         *
         * @param request get item request DTO
         * @return GetItemResponse
         */
        Dto::DynamoDb::GetItemResponse GetItem(const Dto::DynamoDb::GetItemRequest &request) const;

        /**
         * @brief Puts an item
         *
         * @param request put item request DTO
         * @return GetItemResponse
         */
        Dto::DynamoDb::PutItemResponse PutItem(const Dto::DynamoDb::PutItemRequest &request) const;

        /**
         * @brief Query the database
         *
         * @param request query item request DTO
         * @return QueryResponse
         */
        Dto::DynamoDb::QueryResponse Query(const Dto::DynamoDb::QueryRequest &request) const;

        /**
         * @brief Scan the database
         *
         * @param request scan request DTO
         * @return ScanResponse
         */
        Dto::DynamoDb::ScanResponse Scan(const Dto::DynamoDb::ScanRequest &request) const;

        /**
         * @brief Deletes a item
         *
         * @param request delete item request DTO
         * @return DeleteItemResponse
         */
        Dto::DynamoDb::DeleteItemResponse DeleteItem(const Dto::DynamoDb::DeleteItemRequest &request) const;

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
        Dto::DynamoDb::DynamoDbResponse SendDynamoDbRequest(const std::string &body, const std::map<std::string, std::string> &headers) const;

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
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_DYNAMODB_SERVICE_H
