//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_ENVIRONMENT_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_ENVIRONMENT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
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
    struct AddEnvironmentRequest final : Common::BaseCounter<AddEnvironmentRequest> {

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

        friend AddEnvironmentRequest tag_invoke(boost::json::value_to_tag<AddEnvironmentRequest>, boost::json::value const &v) {
            AddEnvironmentRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.environmentKey = Core::Json::GetStringValue(v, "Key");
            r.environmentValue = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddEnvironmentRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"Key", obj.environmentKey},
                    {"Value", obj.environmentValue},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_ENVIRONMENT_REQUEST_H
