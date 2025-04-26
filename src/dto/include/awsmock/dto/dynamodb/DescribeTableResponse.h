//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// Boost include<
#include <boost/beast.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/dynamodb/model/ProvisionedThroughput.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;

    /**
     * @brief DynamoDB describe table response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeTableResponse final : Common::BaseDto<DescribeTableResponse> {

        /**
         * Region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Table size
         */
        long tableSize;

        /**
         * Item count
         */
        long itemCount;

        /**
         * Table ARN
         */
        std::string tableArn;

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
         * Provisioned throughput
         */
        ProvisionedThroughput provisionedThroughput;

        /**
         * Table status
         */
        TableStatusType tableStatus;

        /**
         * HTTP response body
         */
        std::string body;

        /**
         * HTTP response headers
         */
        std::map<std::string, std::string> headers;

        /**
         * HTTP status from docker image
         */
        http::status status;

        /**
         * Creates date time
         */
        system_clock::time_point createdDateTime;

        /**
         * @brief Scans the response and fills in the attributes
         */
        void PrepareResponse();

        /**
         * @brief Parse a JSON stream
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

#endif// AWSMOCK_DTO_DYNAMODB_DESCRIBE_TABLE_RESPONSE_H
