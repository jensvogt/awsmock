//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_ITEM_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_GET_ITEM_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>
#include <awsmock/dto/dynamodb/GetItemKey.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get item request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetItemRequest final : Common::BaseRequest<GetItemRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Keys
         */
        std::map<std::string, AttributeValue> keys;

        /**
         * Projection exception
         */
        std::string projectionExpression;

        /**
         * Consistent read
         */
        bool consistentRead;

        /**
         * Return consumed capacity
         */
        bool returnConsumedCapacity;

        /**
         * Original HTTP request body
         */
        std::string body;

        /**
         * Original HTTP request headers
         */
        std::map<std::string, std::string> headers;

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
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_ITEM_REQUEST_H
