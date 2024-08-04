//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::Transfer {

    struct CreateUserRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * User name
         */
        std::string userName;

        /**
         * Password
         */
        std::string password;

        /**
         * Home directory
         */
        std::string homeDirectory;

        /**
         * Creates a JSON string from the object.
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Parse a JSON stream
         *
         * @param body json input stream
         */
        void FromJson(const std::string &body);

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const CreateUserRequest &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_CREATE_USER_REQUEST_H
