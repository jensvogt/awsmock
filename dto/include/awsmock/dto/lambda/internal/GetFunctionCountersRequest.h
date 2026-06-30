//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Lambda {

    struct GetFunctionCountersRequest final : Common::BaseCounter<GetFunctionCountersRequest> {

        /**
         * @brief Lambda ARN
         */
        std::string lambdaArn;

      private:

        friend GetFunctionCountersRequest tag_invoke(boost::json::value_to_tag<GetFunctionCountersRequest>, boost::json::value const &v) {
            GetFunctionCountersRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetFunctionCountersRequest const &obj) {
            jv = {
                    {"lambdaArn", obj.lambdaArn},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
