//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_TAIL_FUNCTION_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_TAIL_FUNCTION_COUNTERS_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct TailLambdaLogsRequest final : Common::BaseCounter<TailLambdaLogsRequest> {

        /**
         * AWS lambda function ARN
         */
        std::string lambdaArn;

        /**
         * number of lines
         */
        long numLines{};

      private:

        friend TailLambdaLogsRequest tag_invoke(boost::json::value_to_tag<TailLambdaLogsRequest>, boost::json::value const &v) {
            TailLambdaLogsRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            r.numLines = Core::Json::GetLongValue(v, "numLines");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TailLambdaLogsRequest const &obj) {
            jv = {
                    {"lambdaArn", obj.lambdaArn},
                    {"numLines", obj.numLines},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_TAIL_FUNCTION_COUNTERS_REQUEST_H
