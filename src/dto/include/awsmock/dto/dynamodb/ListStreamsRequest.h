//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_STREAMS_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_LIST_STREAMS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list streams for a table request
     *
     * Example:
     * @code(.json)
     * {
     *   "ExclusiveStartStreamArn": "string",
     *   "Limit": number,
     *   "TableName": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListStreamsRequest final : Common::BaseCounter<ListStreamsRequest> {

        /**
         * If this parameter is provided, then only the streams associated with this table name are returned.
         */
        std::string tableName;

        /**
         * The maximum number of streams to return. The upper limit is 100.
         */
        long limit{};

        /**
         * The ARN (Amazon Resource Name) of the first item that this operation will evaluate. Use the value
         * that was returned for LastEvaluatedStreamArn in the previous operation.
         */
        std::string exclusiveStartStreamArn;

      private:

        friend ListStreamsRequest tag_invoke(boost::json::value_to_tag<ListStreamsRequest>, boost::json::value const &v) {
            ListStreamsRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            r.limit = Core::Json::GetLongValue(v, "Limit", 100);
            r.exclusiveStartStreamArn = Core::Json::GetStringValue(v, "ExclusiveStartStreamArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListStreamsRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"Limit", obj.limit},
                    {"ExclusiveStartStreamArn", obj.exclusiveStartStreamArn},
            };
        }
    };
}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_STREAMS_REQUEST_H
