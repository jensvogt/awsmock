//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_ENVIRONMENT_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_ENVIRONMENT_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Update function tag request
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionArn": "string",
     *   "Key": string,
     *   "Value": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateFunctionEnvironmentRequest final : Common::BaseCounter<UpdateFunctionEnvironmentRequest> {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Environment key
         */
        std::string environmentKey;

        /**
         * Environment value
         */
        std::string environmentValue;

      private:

        friend UpdateFunctionEnvironmentRequest tag_invoke(boost::json::value_to_tag<UpdateFunctionEnvironmentRequest>, boost::json::value const &v) {
            UpdateFunctionEnvironmentRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.environmentKey = Core::Json::GetStringValue(v, "environmentKey");
            r.environmentValue = Core::Json::GetStringValue(v, "environmentValue");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateFunctionEnvironmentRequest const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
                    {"environmentKey", obj.environmentKey},
                    {"environmentValue", obj.environmentValue},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_ENVIRONMENT_REQUEST_H
