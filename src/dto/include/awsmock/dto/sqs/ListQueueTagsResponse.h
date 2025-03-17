//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_RESPONSE_H
#define AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_RESPONSE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief List SQS queues tags response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListQueueTagsResponse {

        /**
         * @brief List of queues tags
         */
        std::map<std::string, std::string> tags;

        /**
         * @brief Total number of tags
         */
        long total;

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
        friend std::ostream &operator<<(std::ostream &os, const ListQueueTagsResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_LIST_QUEUE_TAGS_RESPONSE_H
