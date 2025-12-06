//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_ROTATE_SECRET_REQUEST_H
#define AWSMOCK_CORE_DTO_ROTATE_SECRET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/secretsmanager/model/RotationRules.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Rotate a secret value request.
     *
     * Example:
     * @code{.json}
     * {
     *   "ClientRequestToken": "string",
     *   "RotateImmediately": boolean,
     *   "RotationLambdaARN": "string",
     *   "RotationRules": {
     *     "AutomaticallyAfterDays": number,
     *     "Duration": "string",
     *     "ScheduleExpression": "string"
     *   },
     *   "SecretId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RotateSecretRequest final : Common::BaseCounter<RotateSecretRequest> {

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
        std::string rotationLambdaARN;

        /**
         * Rotate immediately
         */
        bool rotateImmediately = false;

        /**
         * Rotation rules
         */
        RotationRules rotationRules;

      private:

        friend RotateSecretRequest tag_invoke(boost::json::value_to_tag<RotateSecretRequest>, boost::json::value const &v) {
            RotateSecretRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.clientRequestToken = Core::Json::GetStringValue(v, "ClientRequestToken");
            r.rotationLambdaARN = Core::Json::GetStringValue(v, "RotationLambdaARN");
            r.rotateImmediately = Core::Json::GetBoolValue(v, "RotateImmediately");
            if (Core::Json::AttributeExists(v, "RotationRules")) {
                r.rotationRules = boost::json::value_to<RotationRules>(v.at("RotationRules"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RotateSecretRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"ClientRequestToken", obj.clientRequestToken},
                    {"RotationLambdaARN", obj.rotationLambdaARN},
                    {"RotateImmediately", obj.rotateImmediately},
                    {"RotationRules", boost::json::value_from(obj.rotationRules)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_CORE_DTO_ROTATE_SECRET_REQUEST_H
