//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_ENVIRONMENT_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_ENVIRONMENT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Add function tag request
     *
     * Example:
     * @code{.json}
     * {
     *   "FunctionArn": "string" ,
     *   "Key": string,
     *   "Value": string
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AddFunctionEnvironmentRequest final : Common::BaseCounter<AddFunctionEnvironmentRequest> {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Environment key
         */
        std::string environmentKey;

        /**
         * Tag value
         */
        std::string environmentValue;

      private:

        friend AddFunctionEnvironmentRequest tag_invoke(boost::json::value_to_tag<AddFunctionEnvironmentRequest>, boost::json::value const &v) {
            AddFunctionEnvironmentRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.environmentKey = Core::Json::GetStringValue(v, "Key");
            r.environmentValue = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddFunctionEnvironmentRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"Key", obj.environmentKey},
                    {"Value", obj.environmentValue},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_ENVIRONMENT_REQUEST_H
