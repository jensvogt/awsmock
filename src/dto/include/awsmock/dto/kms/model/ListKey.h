//
// Created by vogje01 on 4/26/24.
//

#ifndef AWSMOCK_DTO_KMS_LIST_KEY_H
#define AWSMOCK_DTO_KMS_LIST_KEY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/kms/model/KeyState.h>

namespace AwsMock::Dto::KMS {

    /**
     * @brief KMS metadata
     *
     * Example:
     * @code{.json}
     * {
     *   "KeyArn": "string",
     *   "KeyId": "string",
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListKey {

        /**
         * Key ID
         */
        std::string keyId;

        /**
         * Arn
         */
        std::string keyArn;

        /**
         * Key state
         */
        KeyState keyState;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const ListKey &r);
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_DTO_KMS_LIST_KEY_H
