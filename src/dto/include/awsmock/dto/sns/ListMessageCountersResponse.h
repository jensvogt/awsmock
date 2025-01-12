//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_MESSAGE_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SNS_LIST_MESSAGE_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sns/model/Message.h>

namespace AwsMock::Dto::SNS {

    struct ListMessageCountersResponse {

        /**
         * List of queues messages
         */
        std::vector<Message> messages;

        /**
         * Total number of messages
         */
        long total = 0;

        /**
         * Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

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
        friend std::ostream &operator<<(std::ostream &os, const ListMessageCountersResponse &r);
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_MESSAGE_COUNTERS_RESPONSE_H
