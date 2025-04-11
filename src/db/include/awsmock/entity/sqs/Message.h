//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_MESSAGE_H
#define AWSMOCK_DB_ENTITY_SQS_MESSAGE_H

// C++ includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/NumberUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/entity/sqs/MessageAttribute.h>
#include <awsmock/entity/sqs/MessageStatus.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::SQS {

    using std::chrono::system_clock;

    /**
     * @brief SQS message entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Message {

        /**
         * ID
         */
        std::string oid;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Queue name
         */
        std::string queueName;

        /**
         * Message body
         */
        std::string body;

        /**
         * Status
         */
        MessageStatus status = MessageStatus::INITIAL;

        /**
         * Last send datetime
         */
        system_clock::time_point reset;

        /**
         * Send retries
         */
        int retries = 0;

        /**
         * Message size
         */
        long size = 0;

        /**
         * Visibility timeout
         */
        long visibilityTimeout = 30;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Receipt handle
         */
        std::string receiptHandle;

        /**
         * MD5 sum body
         */
        std::string md5Body;

        /**
         * MD5 sum sqs user attributes
         */
        std::string md5MessageAttributes;

        /**
         * MD5 sum sqs system attributes
         */
        std::string md5SystemAttributes;

        /**
         * Attributes
         *
         * Fixed system attributes:
         *
         * - ApproximateReceiveCount
         * - ApproximateFirstReceiveTimestamp Java UNIX timestamp (milliseconds since 01.01.1970)
         * - MessageDeduplicationId
         * - MessageGroupId
         * - SenderId
         * - SentTimestamp Java UNIX timestamp (milliseconds since 01.01.1970)
         * - SequenceNumber
         * - SequenceNumber
         */
        std::map<std::string, std::string> attributes;

        /**
         * List of message attributes.
         *
         * These are the user contributed message attributes.
         */
        MessageAttributeList messageAttributes;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified;

        /**
         * @brief Checks for the existence of a attribute with the given key.
         *
         * @param key attribute key
         * @return true if attribute with the given key exists
         */
        bool HasAttribute(const std::string &key);

        /**
         * @brief Returns the given attribute as integer
         *
         * @param key attribute key
         * @return true if attribute with the given key exists
         */
        int GetIntAttribute(const std::string &key);

        /**
         * @brief Removes an attribute
         *
         * @param key attribute key
         * @return number of attributes removed
         */
        int RemoveAttribute(const std::string &key);

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the DTO to a JSON string representation.
         *
         * @return DTO as JSON string
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
         * @param os output stream
         * @param m message
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Message &m);
    };

    typedef std::vector<Message> MessageList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_H
