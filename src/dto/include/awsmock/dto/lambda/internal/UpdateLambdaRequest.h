//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_UPDATE_LAMBDA_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_UPDATE_LAMBDA_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Update lambda function request
     *
     * Example:
     * @code{.json}
     * {
     *   "functionArn": "string",
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
        std::string functionArn;

        /**
         * Enabled
         */
        bool enabled{};

      private:

        friend UpdateLambdaRequest tag_invoke(boost::json::value_to_tag<UpdateLambdaRequest>, boost::json::value const &v) {
            UpdateLambdaRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.enabled = Core::Json::GetBoolValue(v, "enabled");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UpdateLambdaRequest const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
                    {"enabled", obj.enabled},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_UPDATE_LAMBDA_REQUEST_H
