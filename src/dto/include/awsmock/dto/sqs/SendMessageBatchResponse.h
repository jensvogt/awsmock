//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_RESPONSE_H
#define AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_RESPONSE_H

// C++ standard includes
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

// Poco includes
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

#include <Poco/DateTime.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/XML/XMLWriter.h>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/sqs/model/MessageFailed.h>
#include <awsmock/dto/sqs/model/MessageSuccessful.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief Send message batch response
     *
     * Example:
     * @code{.json}
     * {
     *   "Failed": [
     *      {
     *         "Code": "string",
     *         "Id": "string",
     *         "Message": "string",
     *         "SenderFault": boolean
     *      }
     *   ],
     *   "Successful": [
     *      {
     *         "Id": "string",
     *         "MD5OfMessageAttributes": "string",
     *         "MD5OfMessageBody": "string",
     *         "MD5OfMessageSystemAttributes": "string",
     *         "MessageId": "string",
     *         "SequenceNumber": "string"
     *      }
     *   ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     *
     */
    struct SendMessageBatchResponse {

        /**
         * Successful messages
         */
        std::vector<Dto::SQS::MessageSuccessful> successful;

        /**
         * Failed messages
         */
        std::vector<Dto::SQS::MessageFailed> failed;

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
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const SendMessageBatchResponse &r);
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SEND_MESSAGE_BATCH_RESPONSE_H