//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_PROTOCOL_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_LIST_PROTOCOL_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/transfer/model/Protocol.h>
#include <awsmock/dto/transfer/model/User.h>

namespace AwsMock::Dto::Transfer {

    struct ProtocolCounter {

        /**
         * Protocol
         */
        ProtocolType protocol;

        /**
         * Port
         */
        int port;

        /**
         * @brief Crete JSON document from object
         *
         * @return JSON document
         */
        view_or_value<view, value> ToDocument() const {

            try {

                auto protocolDoc = document{};
                Core::Bson::BsonUtils::SetStringValue(protocolDoc, "Protocol", ProtocolTypeToString(protocol));
                Core::Bson::BsonUtils::SetIntValue(protocolDoc, "Port", port);
                return protocolDoc.extract();

            } catch (const std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }
    };

    struct ListProtocolCountersResponse {

        /**
         * List of attribute counters
         */
        std::vector<ProtocolCounter> protocolCounters;

        /**
         * Total number of users
         */
        long total = 0;

        /**
         * @brief Convert to JSON representation
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
        friend std::ostream &operator<<(std::ostream &os, const ListProtocolCountersResponse &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_PROTOCOL_COUNTERS_RESPONSE_H
