//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserCountersRequest final : Common::BaseCounter<ListUserCountersRequest> {

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User pool name prefix
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

        friend ListUserCountersRequest tag_invoke(boost::json::value_to_tag<ListUserCountersRequest>, boost::json::value const &v) {
            ListUserCountersRequest r;
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListUserCountersRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userPoolId", obj.userPoolId},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_REQUEST_H
