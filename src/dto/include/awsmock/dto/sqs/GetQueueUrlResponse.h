//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_URL_RESPONSE_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_URL_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/core/exception/ServiceException.h"
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/XmlUtils.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Get SQS queue URL response.
     *
     * Example:
     * @code{.xml}
     * <GetQueueUrlResponse xmlns="http://queue.amazonaws.com/doc/2012-11-05/">
     *     <GetQueueUrlResult>
     *         <QueueUrl>https://sqs.us-east-1.amazonaws.com/177715257436/MyQueue</QueueUrl>
     *     </GetQueueUrlResult>
     *     <ResponseMetadata>
     *         <RequestId>552d6f30-4c8e-5b32-aaed-33408c7d6c38</RequestId>
     *     </ResponseMetadata>
     * </GetQueueUrlResponse>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetQueueUrlResponse {

        /**
         * Name of the queue
         */
        std::string queueUrl;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const GetQueueUrlResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_URL_RESPONSE_H
