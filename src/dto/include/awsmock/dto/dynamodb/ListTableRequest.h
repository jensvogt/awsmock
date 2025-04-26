//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableRequest final : Common::BaseDto<ListTableRequest> {

        /**
         * AWS region
         */
        std::string region;

        /**
         * The first table name that this operation will evaluate. Use the value that was returned for LastEvaluatedTableName
         * in a previous operation so that you can obtain the next page of results.
         */
        std::string exclusiveStartTableName;

        /**
         * Limit, default is 100
         */
        int limit = 100;

        /**
         * Original HTTP request body
         */
        std::string body;

        /**
         * Original HTTP request headers
         */
        std::map<std::string, std::string> headers;

        /**
         * @brief Prepares the request to be sent to the DynamoDB container
         */
        void PrepareRequest();

        /**
         * @brief Parse a JSON stream
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_REQUEST_H
