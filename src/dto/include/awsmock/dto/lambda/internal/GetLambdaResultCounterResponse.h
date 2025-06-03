//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_GET_RESULT_COUNTER_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_GET_RESULT_COUNTER_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/lambda/model/LambdaResultCounter.h>

namespace AwsMock::Dto::Lambda {

    struct GetLambdaResultCounterResponse final : Common::BaseCounter<GetLambdaResultCounterResponse> {

        /**
         * Result counter
         */
        LambdaResultCounter lambdaResultCounter;

      private:

        friend GetLambdaResultCounterResponse tag_invoke(boost::json::value_to_tag<GetLambdaResultCounterResponse>, boost::json::value const &v) {
            GetLambdaResultCounterResponse r;
            r.lambdaResultCounter = boost::json::value_to<LambdaResultCounter>(v.at("lambdaResultCounter"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetLambdaResultCounterResponse const &obj) {
            jv = {
                    {"lambdaResultCounter", boost::json::value_from(obj.lambdaResultCounter)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_GET_RESULT_COUNTER_RESPONSE_H
