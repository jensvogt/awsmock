//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DELETE_ALL_ITEM_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_DELETE_ALL_ITEM_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/dynamodb/model/Key.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB delete item request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteAllItemsRequest final : Common::BaseCounter<DeleteAllItemsRequest> {

        /**
         * Table name
         */
        std::string tableName;

      private:

        friend DeleteAllItemsRequest tag_invoke(boost::json::value_to_tag<DeleteAllItemsRequest>, boost::json::value const &v) {
            DeleteAllItemsRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteAllItemsRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_REQUEST_H
