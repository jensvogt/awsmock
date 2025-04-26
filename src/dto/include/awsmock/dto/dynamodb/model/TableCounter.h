//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_TABLE_COUNTER_H
#define AWSMOCK_DTO_DYNAMODB_TABLE_COUNTER_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    using std::chrono::system_clock;

    struct TableCounter final : Common::BaseCounter<TableCounter> {

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Number of objects keys
         */
        long items;

        /**
         * Bucket size in bytes
         */
        long size;

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
            r.tableName = v.at("tableName").as_string();
            r.items = v.at("items").as_int64();
            r.size = v.at("pageSize").as_int64();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TableCounter const &obj) {
            jv = {
                    {"tableName", obj.tableName},
                    {"items", obj.items},
                    {"size", obj.size},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_TABLE_COUNTER_H
