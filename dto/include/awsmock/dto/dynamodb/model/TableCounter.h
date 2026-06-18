//
// Created by vogje01 on 5/10/24.
//

#pragma once

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::DynamoDb {

    struct TableCounter final : Common::BaseObject<TableCounter> {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * Table AWS ARN
         */
        std::string tableArn;

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
            r.region = Core::Json::GetStringValue(v, "region");
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.tableArn = Core::Json::GetStringValue(v, "tableArn");
            r.status = Core::Json::GetStringValue(v, "status");
            r.items = Core::Json::GetLongValue(v, "items");
            r.size = Core::Json::GetLongValue(v, "size");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TableCounter const &obj) {
            jv = {
                    {"tableName", obj.tableName},
                    {"tableArn", obj.tableArn},
                    {"status", obj.status},
                    {"items", obj.items},
                    {"size", obj.size},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };
}// namespace Awsmock::Dto::DynamoDb
