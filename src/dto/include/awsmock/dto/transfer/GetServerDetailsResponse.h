//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/transfer/model/Server.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Get transfer server details response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetServerDetailsResponse {

        /**
         * Region
         */
        Server server;

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
        friend std::ostream &operator<<(std::ostream &os, const GetServerDetailsResponse &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_GET_SERVER_DETAILS_RESPONSE_H
