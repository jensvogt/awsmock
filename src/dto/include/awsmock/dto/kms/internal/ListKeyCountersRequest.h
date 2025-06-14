//
// Created by vogje01 on 4/25/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEY_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_KMS_LIST_KEY_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief List all KMS keys request
     *
     * Example:
     * @code{.json}
     * {
     *   "Limit": number,
     *   "Marker": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKeyCountersRequest final : Common::BaseCounter<ListKeyCountersRequest> {

        /**
         * Region
         */
        std::string region;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        long pageSize = 10;

        /**
         * Skip
         */
        long pageIndex = 0;

        /**
         * @brief List of sort columns names
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListKeyCountersRequest tag_invoke(boost::json::value_to_tag<ListKeyCountersRequest>, boost::json::value const &v) {
            ListKeyCountersRequest r;
            r.region = Core::Json::GetStringValue(v, "region");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListKeyCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_LIST_KEY_COUNTERS_REQUEST_H
