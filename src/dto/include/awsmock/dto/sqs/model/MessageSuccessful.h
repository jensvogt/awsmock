//
// Created by vogje01 on 8/23/24.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_ENTRY_SUCCESS_H
#define AWSMOCK_DTO_SQS_MESSAGE_ENTRY_SUCCESS_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief SQS message entry success
     *
     * @code{.json}
     * {
     *   "Id": "string",
     *   "MD5OfMessageAttributes": "string",
     *   "MD5OfMessageBody": "string",
     *   "MD5OfMessageSystemAttributes": "string",
     *   "MessageId": "string",
     *   "SequenceNumber": "string"
     * }
     * @endcode
     */
    struct MessageSuccessful {

        /**
         * Message ID
         */
        std::string id;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Sequence
         */
        std::string sequenceNumber;

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
         * @brief Converts the DTO to a JSON string.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document);

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
        friend std::ostream &operator<<(std::ostream &os, const MessageSuccessful &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ENTRY_SUCCESS_H
