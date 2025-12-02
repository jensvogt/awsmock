//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/common/SortColumn.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::Lambda {

    struct ListFunctionCountersRequest final : Common::BaseCounter<ListFunctionCountersRequest> {

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        long pageSize{};

        /**
         * Page index
         */
        long pageIndex{};

        /**
         * List of sort columns names
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListFunctionCountersRequest tag_invoke(boost::json::value_to_tag<ListFunctionCountersRequest>, boost::json::value const &v) {
            ListFunctionCountersRequest r;
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListFunctionCountersRequest const &obj) {
            jv = {
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_REQUEST_H
