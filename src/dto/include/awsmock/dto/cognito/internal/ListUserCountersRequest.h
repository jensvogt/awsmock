//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_COGNITO_LIST_USER_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>
#include <awsmock/utils/SortColumn.h>

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
        int pageSize{};

        /**
         * Page index
         */
        int pageIndex{};

        /**
         * Sort columns
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListUserCountersRequest tag_invoke(boost::json::value_to_tag<ListUserCountersRequest>, boost::json::value const &v) {
            ListUserCountersRequest r;
            r.userPoolId = v.at("userPoolId").as_string();
            r.prefix = v.at("prefix").as_string();
            r.pageSize = v.at("pageSize").as_int64();
            r.pageIndex = v.at("pageIndex").as_int64();
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
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
