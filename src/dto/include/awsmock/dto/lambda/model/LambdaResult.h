//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_RESULT_H
#define AWSMOCK_DTO_LAMBDA_RESULT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Lambda code DTO
     *
     * Example:
     * @code{.json}
     * "Code": {
     *   "ImageUri": "string",
     *   "S3Bucket": "string",
     *   "S3Key": "string",
     *   "S3ObjectVersion": "string",
     *   "ZipFile": blob
     * },
     * @endcode
     *
     */
    struct LambdaResult final : Common::BaseCounter<LambdaResult> {

        /**
         * Function ARN
         */
        std::string functionArn;

        /**
         * Lambda response body
         */
        std::string responseBody;

        /**
         * HTTP status code
         */
        int status{};

      private:

        friend LambdaResult tag_invoke(boost::json::value_to_tag<LambdaResult>, boost::json::value const &v) {
            LambdaResult r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.responseBody = Core::Json::GetStringValue(v, "responseBody");
            r.status = Core::Json::GetIntValue(v, "status");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LambdaResult const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
                    {"responseBody", obj.responseBody},
                    {"status", obj.status},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CODE_H
