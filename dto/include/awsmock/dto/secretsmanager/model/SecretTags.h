//
// Created by vogje01 on 4/9/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_SECRET_TAGS_H
#define AWSMOCK_DTO_SECRETSMANAGER_SECRET_TAGS_H

// C++ standard includes
#include <map>
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>

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
    struct SecretTags {

        /**
         * Map of Tags
         */
        std::map<std::string, std::string> tags;

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

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const SecretTags &r);
    };

}// namespace AwsMock::Dto::SecretsManager
#endif// AWSMOCK_DTO_SECRETSMANAGER_SECRET_TAGS_H
