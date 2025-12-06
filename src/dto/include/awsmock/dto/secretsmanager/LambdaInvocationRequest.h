//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETMANAGER_LAMBDA_INVOCATION_REQUEST_H
#define AWSMOCK_DTO_SECRETMANAGER_LAMBDA_INVOCATION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Invoke a lambda function to rotate a secret value.
     *
     * Example:
     * @code{.json}
     * {
     *   "SecretId": "string",
     *   "ClientRequestToken": "string"
     *   "Step": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LambdaInvocationRequest final : Common::BaseCounter<LambdaInvocationRequest> {

        /**
         * Secret ID: ARN or name
         */
        std::string secretId;

        /**
         * Client request token
         */
        std::string clientRequestToken;

        /**
         * Rotation lambda ARN
         */
        std::string step;

      private:

        friend LambdaInvocationRequest tag_invoke(boost::json::value_to_tag<LambdaInvocationRequest>, boost::json::value const &v) {
            LambdaInvocationRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.clientRequestToken = Core::Json::GetStringValue(v, "ClientRequestToken");
            r.step = Core::Json::GetStringValue(v, "Step");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LambdaInvocationRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"ClientRequestToken", obj.clientRequestToken},
                    {"Step", obj.step},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETMANAGER_LAMBDA_INVOCATION_REQUEST_H
