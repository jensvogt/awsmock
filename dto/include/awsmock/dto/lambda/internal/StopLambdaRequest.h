//
// Created by vogje01 on 31/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Lambda {

    /**
     * @brief Stop function request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StopLambdaRequest final : Common::BaseCounter<StopLambdaRequest> {

        /**
         * Function ARN
         */
        std::string lambdaArn;

      private:

        friend StopLambdaRequest tag_invoke(boost::json::value_to_tag<StopLambdaRequest>, boost::json::value const &v) {
            StopLambdaRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StopLambdaRequest const &obj) {
            jv = {
                    {"lambdaArn", obj.lambdaArn},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
