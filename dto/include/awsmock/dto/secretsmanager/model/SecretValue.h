//
// Created by vogje01 on 4/9/24.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::SecretsManager {

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
    struct SecretValue final : Common::BaseObject<SecretValue> {

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
                    {"Key", obj.secretKey},
                    {"Value", obj.secretValue},
            };
        }
    };
}// namespace Awsmock::Dto::SecretsManager
