//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_REQUEST_H

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
    struct DeleteItemRequest final : Common::BaseCounter<DeleteItemRequest> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Item key
         */
        std::map<std::string, AttributeValue> keys;

      private:

        friend DeleteItemRequest tag_invoke(boost::json::value_to_tag<DeleteItemRequest>, boost::json::value const &v) {
            DeleteItemRequest r;
            r.tableName = Core::Json::GetStringValue(v, "TableName");
            if (Core::Json::AttributeExists(v, "Key")) {
                for (boost::json::object keyObject = v.at("Key").as_object(); const auto &key: keyObject) {
                    const auto value = boost::json::value_to<AttributeValue>(key.value());
                    r.keys[key.key()] = value;
                }
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteItemRequest const &obj) {
            jv = {
                    {"TableName", obj.tableName},
                    {"Key", boost::json::value_from(obj.keys)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_DELETE_ITEM_REQUEST_H
