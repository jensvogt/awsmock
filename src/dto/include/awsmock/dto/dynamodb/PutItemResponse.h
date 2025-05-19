//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_PUT_ITEM_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_PUT_ITEM_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/dynamodb/model/TableStatus.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Put item response
     *
     * Example:
     * @code{.json}
     * {
     *   "ConsumedCapacity":
     *     {
     *       "TableName":"test-table",
     *       "CapacityUnits":1.0
     *     }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutItemResponse final : Common::BaseCounter<PutItemResponse> {

        /**
         * Table name
         */
        std::string tableName;

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
        boost::beast::http::status status = boost::beast::http::status::unknown;

      private:

        friend PutItemResponse tag_invoke(boost::json::value_to_tag<PutItemResponse>, boost::json::value const &v) {
            PutItemResponse r{};
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutItemResponse const &obj) {
            jv = {
                    {"TableName", obj.tableName},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_PUT_ITEM_RESPONSE_H
