//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_RESPONSE_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/beast.hpp>

// AwsMock includes
#include "model/AttributeValue.h"


#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Dto::DynamoDb {

    namespace http = boost::beast::http;
    /**
     * @brief Delete item request DTO
     *
     * Example:
     * @code{.json}
     * {
     *   "ConsumedCapacity":
     *     {
     *       "TableName":"test-table",
     *       "CapacityUnits":1.0
     *    }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteItemResponse final : Common::BaseCounter<DeleteItemResponse> {

        /**
         * Item array
         */
        std::map<std::string, AttributeValue> attributes;

      private:

        friend DeleteItemResponse tag_invoke(boost::json::value_to_tag<DeleteItemResponse>, boost::json::value const &v) {
            DeleteItemResponse r;
            if (Core::Json::AttributeExists(v, "Attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, AttributeValue>>(v.at("Attributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteItemResponse const &obj) {
            jv = {
                    {"Attributes", boost::json::value_from(obj.attributes)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_RESPONSE_H
