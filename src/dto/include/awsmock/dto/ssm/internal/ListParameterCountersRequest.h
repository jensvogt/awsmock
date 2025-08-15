//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SSM_LIST_PARAMETER_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SSM_LIST_PARAMETER_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief List all parameters request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListParameterCountersRequest final : Common::BaseCounter<ListParameterCountersRequest> {

        /**
         * Prefix
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
         * Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListParameterCountersRequest tag_invoke(boost::json::value_to_tag<ListParameterCountersRequest>, boost::json::value const &v) {
            ListParameterCountersRequest r;
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListParameterCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_LIST_PARAMETER_COUNTERS_REQUEST_H
