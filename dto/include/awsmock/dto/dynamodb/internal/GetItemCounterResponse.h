//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_ITEM_COUNTER_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_GET_ITEM_COUNTER_RESPONSE_H

// C++ standard includes
#include <vector>

// AwsMock includes
#include "awsmock/dto/dynamodb/model/ItemCounter.h"


#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/TableCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get item details response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetItemCounterResponse final : Common::BaseCounter<GetItemCounterResponse> {

        /**
         * Table name
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

        /**
         * Item counter
         */
        ItemCounter itemCounter;

      private:

        friend GetItemCounterResponse tag_invoke(boost::json::value_to_tag<GetItemCounterResponse>, boost::json::value const &v) {
            GetItemCounterResponse r;
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.partitionKey = Core::Json::GetStringValue(v, "partitionKey");
            r.sortKey = Core::Json::GetStringValue(v, "sortKey");
            r.itemCounter = boost::json::value_to<ItemCounter>(v.at("itemCounter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetItemCounterResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"tableName", obj.tableName},
                    {"partitionKey", obj.partitionKey},
                    {"sortKey", obj.sortKey},
                    {"itemCounter", boost::json::value_from(obj.itemCounter)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_ITEM_COUNTER_RESPONSE_H
