//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListTableCountersRequest final : Common::BaseCounter<ListTableCountersRequest> {

        /**
         * AWS region
         */
        std::string region;

        /**
         * TAble name prefix.
         */
        std::string prefix;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * Page index
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListTableCountersRequest tag_invoke(boost::json::value_to_tag<ListTableCountersRequest>, boost::json::value const &v) {
            ListTableCountersRequest r;
            r.prefix = v.at("prefix").as_string();
            r.pageSize = v.at("pageSize").as_int64();
            r.pageIndex = v.at("pageIndex").as_int64();

            // Sort columns
            for (const auto &sc: v.at("sortColumns").as_array()) {
                Common::SortColumn sortColumn;
                sortColumn.column = sc.at("column").as_string();
                sortColumn.sortDirection = sc.at("sortDirection").as_int64();
                r.sortColumns.emplace_back(sortColumn);
            }

            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTableCountersRequest const &obj) {
            jv = {
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_LIST_TABLE_COUNTERS_REQUEST_H
