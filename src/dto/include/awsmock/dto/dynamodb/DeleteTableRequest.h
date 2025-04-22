//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB delete table request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteTableRequest final : Common::BaseDto<DeleteTableRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

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

#endif// AWSMOCK_DTO_DYNAMODB_DELETE_TABLE_REQUEST_H
