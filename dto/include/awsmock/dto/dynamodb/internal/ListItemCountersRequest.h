//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListItemCountersRequest final : Common::BaseCounter<ListItemCountersRequest> {

        /**
         * Dynamodb table name
         */
        std::string tableName{};

        /**
         * TAble name prefix.
         */
        std::string prefix;

        /**
         * Page size
         */
        long pageSize = 10;

        /**
         * Page index
         */
        long pageIndex = 0;

        /**
         * Page index
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListItemCountersRequest tag_invoke(boost::json::value_to_tag<ListItemCountersRequest>, boost::json::value const &v) {
            ListItemCountersRequest r;
            r.tableName = Core::Json::GetStringValue(v, "tableName");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListItemCountersRequest const &obj) {
            jv = {
                    {"tableName", obj.tableName},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_ITEM_COUNTERS_REQUEST_H
