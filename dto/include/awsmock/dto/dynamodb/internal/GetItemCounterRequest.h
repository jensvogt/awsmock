//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_ITEM_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_GET_ITEM_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get item details request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetItemCounterRequest final : Common::BaseCounter<GetItemCounterRequest> {

        /**
         * Name of the table
         */
        std::string tableName;

        /**
         * Partition key
         */
        std::string partitionKey;

        /**
         * Sort key
         */
        std::string sortKey;

      private:

        friend GetItemCounterRequest tag_invoke(boost::json::value_to_tag<GetItemCounterRequest>, boost::json::value const &v) {
            GetItemCounterRequest r;
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.partitionKey = Core::Json::GetStringValue(v, "partitionKey");
            r.sortKey = Core::Json::GetStringValue(v, "sortKey");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetItemCounterRequest const &obj) {
            jv = {
                    {"tableName", obj.tableName},
                    {"partitionKey", obj.partitionKey},
                    {"sortKey", obj.sortKey},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_ITEM_COUNTERS_REQUEST_H
