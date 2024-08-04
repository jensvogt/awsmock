//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_USER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_LIST_USER_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Lisr FTP user request
     *
     * Example:
     * @code{.json}
     * {
     *   "MaxResults": 100,
     *   "NextToken": "eyJNYXJrZXIiOiBudWxsLCAiYm90b1X0cnVuU2F0ZV9hbW91bnQiOiAyfQ==",
     *   "ServerId": "s-01234567890abcdef"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUsersRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * Maximal number of results
         */
        int maxResults;

        /**
         * Token
         */
        std::string nextToken;

        /**
         * @brief Parse a JSON stream
         *
         * @param body json input stream
         */
        void FromJson(const std::string &body);

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListUsersRequest &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_USER_REQUEST_H
