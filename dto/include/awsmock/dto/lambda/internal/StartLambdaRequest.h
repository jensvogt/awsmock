//
// Created by vogje01 on 31/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Lambda {

    /**
     * @brief Start function request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StartLambdaRequest final : Common::BaseCounter<StartLambdaRequest> {

        /**
         * Function ARN
         */
        std::string lambdaArn;

      private:

        friend StartLambdaRequest tag_invoke(boost::json::value_to_tag<StartLambdaRequest>, boost::json::value const &v) {
            StartLambdaRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StartLambdaRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"lambdaArn", obj.lambdaArn},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
