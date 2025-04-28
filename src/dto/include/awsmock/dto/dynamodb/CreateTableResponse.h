//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// Boost include<
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;

    /**
     * @brief Create table response DTO
     *
     * Example:
     * @code{.json}
     * {
     *   "TableDescription":
     *     {
     *       "AttributeDefinitions": [
     *           {
     *             "attributeName":"orgaNr",
     *             "AttributeType":"N"
     *           }
     *       ],
     *       "TableName":"test-table",
     *       "KeySchema":[
     *         {
     *           "attributeName":"orgaNr",
     *           "KeyType":"HASH"
     *         }
     *       ],
     *       "TableStatus":"ACTIVE",
     *       "CreationDateTime":1703158038.811,
     *       "ProvisionedThroughput":
     *         {
     *           "LastIncreaseDateTime":0.000,
     *           "LastDecreaseDateTime":0.000,
     *           "NumberOfDecreasesToday":0,
     *           "ReadCapacityUnits":1,
     *           "WriteCapacityUnits":1
     *         },
     *       "TableSizeBytes":0,
     *       "ItemCount":0,
     *       "TableArn":"arn:aws:dynamodb:ddblocal:000000000000:table/test-table",
     *       "DeletionProtectionEnabled":false
     *    }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CreateTableResponse final : Common::BaseDto<CreateTableResponse> {

        /**
         * Region
         */
        std::string region;

        /**
         * Table class
         */
        std::string tableClass;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Table ARN
         */
        std::string tableArn;

        /**
         * Table size
         */
        long tableSizeBytes = 0;

        /**
         * Item count
         */
        long itemCount = 0;

        /**
         * Delete protection enabled
         */
        bool deleteProtectionEnabled;

        /**
         * Key schema
         */
        std::map<std::string, std::string> keySchemas;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Attribute definitions
         */
        std::map<std::string, std::string> attributes;

        /**
         * Table status
         */
        TableStatusType tableStatus;

        /**
         * Original HTTP response body
         */
        std::string body;

        /**
         * Original HTTP response headers
         */
        std::map<std::string, std::string> headers;

        /**
         * HTTP status from docker image
         */
        http::status status;

        /**
         * @brief Scans the response and fills in the attributes
         */
        void ScanResponse();

        /**
         * @brief Parse a JSON string
         *
         * @param body JSON string
         * @param headers map of headers
         */
        void FromJson(const std::string &body, const std::map<std::string, std::string> &headers);

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_CREATE_TABLE_RESPONSE_H
