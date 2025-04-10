//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SEND_MESSAGE_RESPONSE_H
#define AWSMOCK_DTO_SQS_SEND_MESSAGE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * Send message response
     *
     * Example:
     * @code{.json}
     * {
     *   "MD5OfMessageAttributes": "string",
     *   "MD5OfMessageBody": "string",
     *   "MD5OfMessageSystemAttributes": "string",
     *   "MessageId": "string",
     *   "SequenceNumber": "string"
     * }
     * @endcode
     */
    struct SendMessageResponse {

        /**
         * ID
         */
        long id;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Receipt handle
         */
        std::string receiptHandle;

        /**
         * MD5 sum of body
         */
        std::string md5Body;

        /**
         * MD5 sum of sqs of user attributes
         */
        std::string md5MessageAttributes;

        /**
         * MD5 sum of sqs system attributes
         */
        std::string md5SystemAttributes;

        /**
         * Message sequence number
         */
        std::string sequenceNumber = Core::StringUtils::CreateRandomUuid();

        /**
         * Request ID
         */
        std::string requestId;

        /**
         * Attributes
         */
        std::map<std::string, std::string> attributes;

        /**
         * Message attributes
         */
        MessageAttributeList messageAttributes;

        /**
         * Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Convert from JSON representation
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const;

        /**
         * Convert from XML representation
         *
         * @param xmlString  XML string
         */
        void FromXml(const std::string &xmlString);

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
        friend std::ostream &operator<<(std::ostream &os, const SendMessageResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SEND_MESSAGE_RESPONSE_H
