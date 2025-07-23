//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_TAG_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_TAG_REQUEST_H

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
     *   "Key": "string",
     *   "Value": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UpdateFunctionTagRequest final : Common::BaseCounter<UpdateFunctionTagRequest> {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Tag key
         */
        std::string tagKey;

        /**
         * Tag value
         */
        std::string tagValue;

      private:

        friend UpdateFunctionTagRequest tag_invoke(boost::json::value_to_tag<UpdateFunctionTagRequest>, boost::json::value const &v) {
            UpdateFunctionTagRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.tagKey = Core::Json::GetStringValue(v, "Key");
            r.tagValue = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateFunctionTagRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"Key", obj.tagKey},
                    {"Value", obj.tagValue},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_UPDATE_FUNCTION_TAG_REQUEST_H
