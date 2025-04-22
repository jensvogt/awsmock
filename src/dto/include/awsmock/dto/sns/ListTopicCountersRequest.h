//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Dto::SNS {

    struct ListTopicCountersRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Page size
         */
        int pageSize;

        /**
         * Page index
         */
        int pageIndex;

        /**
         * Sort column
         */
        std::vector<Database::SortColumn> sortColumns;

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const ListTopicCountersRequest &r);
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_LIST_TOPIC_COUNTERS_REQUEST_H
