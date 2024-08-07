//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H
#define AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// Poco includes
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/XML/XMLWriter.h>

// AwsMock includes
#include "awsmock/core/exception/ServiceException.h"
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/entity/sqs/Message.h>

namespace AwsMock::Dto::SQS {

    /**
     * Receive message request.
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
        int waitTimeSeconds = 1;

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
         * Converts the JSON string to DTO.
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const ReceiveMessageRequest &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_RECEIVE_MESSAGE_REQUEST_H
