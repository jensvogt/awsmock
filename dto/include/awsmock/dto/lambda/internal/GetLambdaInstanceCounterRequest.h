//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Lambda {

    struct GetLambdaInstanceCounterRequest final : Common::BaseCounter<GetLambdaInstanceCounterRequest> {

        /**
         * @brief Lambda function ARN
         */
        std::string lambdaArn;

        /**
         * @brief Instance ID
         */
        std::string instanceId;

      private:

        friend GetLambdaInstanceCounterRequest tag_invoke(boost::json::value_to_tag<GetLambdaInstanceCounterRequest>, boost::json::value const &v) {
            GetLambdaInstanceCounterRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            r.instanceId = Core::Json::GetStringValue(v, "instanceId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetLambdaInstanceCounterRequest const &obj) {
            jv = {
                    {"lambdaArn", obj.lambdaArn},
                    {"instanceId", obj.instanceId},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
