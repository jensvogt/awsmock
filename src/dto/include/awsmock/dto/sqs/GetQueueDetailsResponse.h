//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <sstream>
#include <string>

// Poco includes
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/XML/XMLWriter.h>

// AwsMock includes
#include "awsmock/core/exception/ServiceException.h"
#include <awsmock/core/JsonUtils.h>
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
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

        /**
         * Convert to a JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const GetQueueDetailsResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H
