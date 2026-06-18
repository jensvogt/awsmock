//
// Created by vogje01 on 5/10/24.
//

#pragma once

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>
#include <awsmock/dto/dynamodb/model/Item.h>

namespace Awsmock::Dto::DynamoDb {

    struct ItemCounter final : Common::BaseObject<ItemCounter> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Item ID
         */
        std::string id;

        /**
         * Item size in bytes
         */
        long size = 0;

        /**
         * Attributes
         */
        std::unordered_map<std::string, AttributeValue> attributes;

        /**
         * Partition key
         */
        std::string partitionKey;

        /**
         * Sort key
         */
        std::string sortKey;

        /**
         * Create timestamp
         */
        system_clock::time_point created;

        /**
         * Create timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Convert to a BSON document
         *
         * @return BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend ItemCounter tag_invoke(boost::json::value_to_tag<ItemCounter>, boost::json::value const &v) {
            ItemCounter r;
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.id = Core::Json::GetStringValue(v, "id");
            r.size = Core::Json::GetLongValue(v, "size");
            r.partitionKey = Core::Json::GetStringValue(v, "partitionKey");
            r.sortKey = Core::Json::GetStringValue(v, "sortKey");
            r.attributes = boost::json::value_to<std::unordered_map<std::string, AttributeValue>>(v.at("attributes"));
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ItemCounter const &obj) {
            jv = {
                    {"tableName", obj.tableName},
                    {"id", obj.id},
                    {"size", obj.size},
                    {"partitionKey", boost::json::value_from(obj.partitionKey)},
                    {"sortKey", boost::json::value_from(obj.sortKey)},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }// namespace Awsmock::Dto::DynamoDb
    };
}// namespace Awsmock::Dto::DynamoDb
