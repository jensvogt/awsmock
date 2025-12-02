//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_DESCRIBE_SECRET_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_DESCRIBE_SECRET_REQUEST_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Describe secret request
     *
     * Example:
     * @code{.json}
     * {
     *   "SecretId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeSecretRequest final : Common::BaseCounter<DescribeSecretRequest> {

        /**
         * Secret ID
         */
        std::string secretId;

      private:

        friend DescribeSecretRequest tag_invoke(boost::json::value_to_tag<DescribeSecretRequest>, boost::json::value const &v) {
            DescribeSecretRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeSecretRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_DESCRIBE_SECRET_REQUEST_H
