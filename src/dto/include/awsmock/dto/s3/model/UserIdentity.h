//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_USER_IDENTITY_H
#define AWSMOCK_DTO_S3_USER_IDENTITY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 object user identity DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserIdentity {

        /**
         * AWS principal ID
         */
        std::string principalId;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

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
        friend std::ostream &operator<<(std::ostream &os, const UserIdentity &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_USER_IDENTITY_H
