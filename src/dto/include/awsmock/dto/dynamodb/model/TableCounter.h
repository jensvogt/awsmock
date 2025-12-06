//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_TABLE_COUNTER_H
#define AWSMOCK_DTO_DYNAMODB_TABLE_COUNTER_H

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    struct TableCounter final : Common::BaseCounter<TableCounter> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Table status
         */
        std::string status;

        /**
         * Number of objects keys
         */
        long items{};

        /**
         * Bucket size in bytes
         */
        long size{};

        /**
         * Create timestamp
         */
        system_clock::time_point created;

        /**
         * Create timestamp
         */
        system_clock::time_point modified;

      private:

        friend TableCounter tag_invoke(boost::json::value_to_tag<TableCounter>, boost::json::value const &v) {
            TableCounter r;
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.status = Core::Json::GetStringValue(v, "status");
            r.items = Core::Json::GetLongValue(v, "items");
            r.size = Core::Json::GetLongValue(v, "size");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TableCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"tableName", obj.tableName},
                    {"status", obj.status},
                    {"items", obj.items},
                    {"size", obj.size},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_TABLE_COUNTER_H
