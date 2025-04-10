//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_TRANSFER_SERVER_H
#define AWSMOCK_DTO_TRANSFER_SERVER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/transfer/model/IdentityProviderDetails.h>
#include <awsmock/dto/transfer/model/IdentityProviderType.h>

namespace AwsMock::Dto::Transfer {

    struct Server {

        /**
         * Region
         */
        std::string region;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Domain
         */
        std::string domain;

        /**
         * Identity provider type
         */
        IdentityProviderType identityProviderType;

        /**
         * Identity provider details
         */
        IdentityProviderDetails identityProviderDetails;

        /**
         * Endpoint type
         */
        std::string endpointType;

        /**
         * Logging role
         */
        std::string loggingRole;

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * State
         */
        std::string state;

        /**
         * User count
         */
        int userCount;

        /**
         * Ports
         */
        std::vector<int> ports;

        /**
         * Concurrency
         */
        int concurrency;

        /**
         * Protocols
         */
        std::vector<std::string> protocols;

        /**
         * Last started
         */
        system_clock::time_point lastStarted;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @param jsonString DTO as JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const Server &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif//AWSMOCK_DTO_TRANSFER_SERVER_H
