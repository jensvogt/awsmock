//
// Created by vogje01 on 4/9/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_SECRET_VALUE_H
#define AWSMOCK_DTO_SECRETSMANAGER_SECRET_VALUE_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secret tags
     *
     * Example:
     * @code{.json}
     * "Tags": [
     *  {
     *    "Key": "string",
     *    "Value": "string"
     *  }
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SecretValue final : Common::BaseCounter<SecretValue> {

        /**
         * Key
         */
        std::string secretKey;

        /**
         * Value
         */
        std::string secretValue;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document);

      private:

        friend SecretValue tag_invoke(boost::json::value_to_tag<SecretValue>, boost::json::value const &v) {
            SecretValue r;
            r.secretKey = Core::Json::GetStringValue(v, "Key");
            r.secretValue = Core::Json::GetStringValue(v, "Value");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SecretValue const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Key", obj.secretKey},
                    {"Value", obj.secretValue},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager
#endif// AWSMOCK_DTO_SECRETSMANAGER_SECRET_VALUE_H
