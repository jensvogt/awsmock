//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    /**
     * @brief Get queue details response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetQueueDetailsResponse {

        /**
         * Region
         */
        std::string region;

        /**
         * Queue name
         */
        std::string queueName;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Number of messages
         */
        long messageCount;

        /**
         * Retention period
         */
        long retentionPeriod;

        /**
         * Maximal message size
         */
        long maxMessageSize;

        /**
         * Visibility timeout
         */
        long visibilityTimeout;

        /**
         * Size of all messages
         */
        long size;

        /**
         * Delay
         */
        long delay;

        /**
         * Queue owner
         */
        std::string owner;

        /**
         * Number of message available
         */
        long available;

        /**
         * Number of message invisible
         */
        long invisible;

        /**
         * Number of message delayed
         */
        long delayed;

        /**
         * Dead letter queue ARN
         */
        std::string dlqArn;

        /**
         * Maximal number of retries
         */
        int dlqMaxReceive;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

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
        friend std::ostream &operator<<(std::ostream &os, const GetQueueDetailsResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H
