//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_UPLOAD_FUNCTION_CODE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_UPLOAD_FUNCTION_CODE_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Create function response
     *
     * Example:
     * @code{.json}
     * {
     *   "functionArn": "string",
     *   "functionCode": "string",
     *   "version": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UploadFunctionCodeRequest final : Common::BaseCounter<UploadFunctionCodeRequest> {

        /**
         * Lambda function ARN
         */
        std::string functionArn;

        /**
         * Base64 encoded function code string
         */
        std::string functionCode;

        /**
         * Version
         */
        std::string version;

      private:

        friend UploadFunctionCodeRequest tag_invoke(boost::json::value_to_tag<UploadFunctionCodeRequest>, boost::json::value const &v) {
            UploadFunctionCodeRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.functionCode = Core::Json::GetStringValue(v, "functionCode");
            r.version = Core::Json::GetStringValue(v, "version");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UploadFunctionCodeRequest const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
                    {"functionCode", obj.functionCode},
                    {"version", obj.version},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_UPLOAD_FUNCTION_CODE_REQUEST_H
