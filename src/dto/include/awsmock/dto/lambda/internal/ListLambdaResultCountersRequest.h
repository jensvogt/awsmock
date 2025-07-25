//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_LAMBDA_RESULT_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_LAMBDA_RESULT_COUNTERS_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief List Lambda result counter request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListLambdaResultCountersRequest final : Common::BaseCounter<ListLambdaResultCountersRequest> {

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
        long pageSize;

        /**
         * Page index
         */
        long pageIndex;

        /**
         * List of sort columns names
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend ListLambdaResultCountersRequest tag_invoke(boost::json::value_to_tag<ListLambdaResultCountersRequest>, boost::json::value const &v) {
            ListLambdaResultCountersRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaResultCountersRequest const &obj) {
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

#endif// AWSMOCK_DTO_LAMBDA_LIST_LAMBDA_RESULT_COUNTERS_REQUEST_H
