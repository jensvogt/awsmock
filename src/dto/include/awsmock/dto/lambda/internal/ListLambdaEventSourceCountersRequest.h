//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>


// AwsMock includes
#include "awsmock/dto/common/SortColumn.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief List Lambda event sources request.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListLambdaEventSourceCountersRequest final : Common::BaseCounter<ListLambdaEventSourceCountersRequest> {

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        int pageSize{};

        /**
         * Page index
         */
        int pageIndex{};

        /**
         * Sort column
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListLambdaEventSourceCountersRequest tag_invoke(boost::json::value_to_tag<ListLambdaEventSourceCountersRequest>, boost::json::value const &v) {
            ListLambdaEventSourceCountersRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaEventSourceCountersRequest const &obj) {
            jv = {
                    {"lambdaArn", obj.lambdaArn},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_COUNTERS_REQUEST_H
