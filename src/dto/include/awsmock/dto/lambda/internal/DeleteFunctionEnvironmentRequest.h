//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_ENVIRONMENT_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_ENVIRONMENT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Delete function tag request
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionArn": "string" ,
     *   "Key": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteFunctionEnvironmentRequest final : Common::BaseCounter<DeleteFunctionEnvironmentRequest> {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Environment key
         */
        std::string environmentKey;

      private:

        friend DeleteFunctionEnvironmentRequest tag_invoke(boost::json::value_to_tag<DeleteFunctionEnvironmentRequest>, boost::json::value const &v) {
            DeleteFunctionEnvironmentRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.environmentKey = Core::Json::GetStringValue(v, "Key");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteFunctionEnvironmentRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"Key", obj.environmentKey},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_ENVIRONMENT_REQUEST_H
