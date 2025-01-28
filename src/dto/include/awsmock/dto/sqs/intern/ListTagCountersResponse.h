//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_TAG_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_TAG_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/TagCounter.h>

namespace AwsMock::Dto::SQS {

    struct ListTagCountersResponse {

        /**
         * List of tag counters
         */
        std::vector<TagCounter> tagCounters;

        /**
         * Total number of queues
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
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListTagCountersResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_TAG_COUNTERS_RESPONSE_H
