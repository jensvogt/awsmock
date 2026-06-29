//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::Lambda {

    /**
     * @brief Update lambda function request
     *
     * Example:
     * @code{.json}
     * {
     *   "lambdaArn": "string",
     *   "enabled": boolean,
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateLambdaRequest final : Common::BaseCounter<UpdateLambdaRequest> {

        /**
         * Lambda function ARN
         */
        std::string lambdaArn;

        /**
         * Enabled
         */
        bool enabled{};

      private:

        friend UpdateLambdaRequest tag_invoke(boost::json::value_to_tag<UpdateLambdaRequest>, boost::json::value const &v) {
            UpdateLambdaRequest r;
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateLambdaRequest const &obj) {
            jv = {
                    {"lambdaArn", obj.lambdaArn},
                    {"enabled", obj.enabled},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
