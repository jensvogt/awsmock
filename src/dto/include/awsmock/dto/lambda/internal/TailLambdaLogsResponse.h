//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/FunctionCounter.h>

namespace AwsMock::Dto::Lambda {

    struct TailLambdaLogsResponse final : Common::BaseCounter<TailLambdaLogsResponse> {

        /**
         * List of buckets
         */
        std::vector<std::string> logs;

      private:

        friend TailLambdaLogsResponse tag_invoke(boost::json::value_to_tag<TailLambdaLogsResponse>, boost::json::value const &v) {
            TailLambdaLogsResponse r;
            r.logs = boost::json::value_to<std::vector<std::string>>(v.at("logs"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TailLambdaLogsResponse const &obj) {
            jv = {
                    {"logs", boost::json::value_from(obj.logs)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_COUNTERS_RESPONSE_H
