//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_API_GATEWAY_LIST_REST_API_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_API_GATEWAY_LIST_REST_API_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::ApiGateway {

    /**
     * @brief List REST API counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListRestApiCountersRequest final : Common::BaseCounter<ListRestApiCountersRequest> {

        /**
         * Name prefix
         */
        std::string prefix;

        /**
         * Maximal number of results
         */
        long pageSize{};

        /**
         * Page index
         */
        long pageIndex{};

        /**
         * Sort columns
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListRestApiCountersRequest tag_invoke(boost::json::value_to_tag<ListRestApiCountersRequest>, boost::json::value const &v) {
            ListRestApiCountersRequest r;
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListRestApiCountersRequest const &obj) {
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

}// namespace AwsMock::Dto::ApiGateway

#endif// AWSMOCK_DTO_API_GATEWAY_LIST_REST_API_COUNTERS_REQUEST_H
