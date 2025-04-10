//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Receive message request.
     *
     * Example:
     * @code{.json}
     * {
     *   "AttributeNames": [ "string" ],
     *   "MaxNumberOfMessages": number,
     *   "MessageAttributeNames": [ "string" ],
     *   "MessageSystemAttributeNames": [ "string" ],
     *   "QueueUrl": "string",
     *   "ReceiveRequestAttemptId": "string",
     *   "VisibilityTimeout": number,
     *   "WaitTimeSeconds": number
     * }
     * @endcode
     *
     * @par
     * The wait time seconds parameter is the duration (in seconds) for which the call waits for a message to arrive in the queue before returning. If a message is available, the call returns sooner
     * than WaitTimeSeconds. If no messages are available and the wait time expires, the call does not return a message list. If you are using the Java SDK, it returns a ReceiveMessageResponse object,
     * which has an empty list instead of a Null object.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ReceiveMessageRequest {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue name
         */
        std::string queueName;

        /**
         * Maximal number of resources
         */
        int maxMessages = 10;

        /**
         * Visibility
         */
        int visibilityTimeout = 15;

        /**
         * Wait time in seconds
         */
        int waitTimeSeconds = 10;

        /**
         * List of attribute names
         */
        std::vector<std::string> attributeNames = {"All"};

        /**
         * List of message attribute names
         */
        std::vector<std::string> messageAttributeNames = {"All"};

        /**
         * Resource
         */
        std::string resource = "SQS";

        /**
         * Resource
         */
        std::string requestId;

        /**
         * @brief Converts the JSON string to DTO.
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const ReceiveMessageRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H
