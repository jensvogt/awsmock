//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_TAG_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_TAG_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
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
    struct AddTagRequest final : Common::BaseCounter<AddTagRequest> {

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

        friend AddTagRequest tag_invoke(boost::json::value_to_tag<AddTagRequest>, boost::json::value const &v) {
            AddTagRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.tagKey = Core::Json::GetStringValue(v, "Key");
            r.tagValue = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddTagRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"Key", obj.tagKey},
                    {"Value", obj.tagValue},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ADD_FUNCTION_TAG_REQUEST_H
