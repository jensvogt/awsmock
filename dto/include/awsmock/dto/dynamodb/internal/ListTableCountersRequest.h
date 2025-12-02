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
         * Table name prefix.
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

        friend ListTableCountersRequest tag_invoke(boost::json::value_to_tag<ListTableCountersRequest>, boost::json::value const &v) {
            ListTableCountersRequest r;
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
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
