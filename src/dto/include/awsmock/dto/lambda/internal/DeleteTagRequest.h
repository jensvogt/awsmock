//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_TAG_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_TAG_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
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
    struct DeleteTagRequest final : Common::BaseCounter<DeleteTagRequest> {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Tag key
         */
        std::string tagKey;

      private:

        friend DeleteTagRequest tag_invoke(boost::json::value_to_tag<DeleteTagRequest>, boost::json::value const &v) {
            DeleteTagRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "FunctionArn");
            r.tagKey = Core::Json::GetStringValue(v, "Key");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteTagRequest const &obj) {
            jv = {
                    {"FunctionArn", obj.functionArn},
                    {"Key", obj.tagKey},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_TAG_REQUEST_H
