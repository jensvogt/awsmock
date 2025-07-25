//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_LAMBDA_RESULT_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_LAMBDA_RESULT_COUNTERS_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct DeleteLambdaResultCountersRequest final : Common::BaseCounter<DeleteLambdaResultCountersRequest> {

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

      private:

        friend DeleteLambdaResultCountersRequest tag_invoke(boost::json::value_to_tag<DeleteLambdaResultCountersRequest>, boost::json::value const &v) {
            DeleteLambdaResultCountersRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteLambdaResultCountersRequest const &obj) {
            jv = {
                    {"lambdaArn", obj.lambdaArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_LAMBDA_RESULT_COUNTERS_REQUEST_H
