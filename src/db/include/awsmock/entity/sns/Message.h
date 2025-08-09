//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SNS_MESSAGE_H
#define AWSMOCK_DB_ENTITY_SNS_MESSAGE_H

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/sns/MessageAttribute.h>
#include <awsmock/entity/sns/MessageStatus.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::SNS {

    /**
     * @brief SNS message entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Message {

        /**
         * ID
         */
        std::string oid;

        /**
         * Aws region name
         */
        std::string region;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Target ARN
         */
        std::string targetArn;

        /**
         * Message body
         */
        std::string message;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Message ID
         */
        MessageStatus status;

        /**
         * List of sqs
         */
        std::map<std::string, MessageAttribute> messageAttributes;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * Message size
         */
        long size;

        /**
         * Last send datetime
         */
        system_clock::time_point lastSend;

        /**
         * Creation datetime
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modified datetime
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the MongoDB document to an entity
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
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @param os output stream
         * @param message message entity
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Message &message);
    };

    typedef Message Message;
    typedef std::vector<Message> MessageList;

}// namespace AwsMock::Database::Entity::SNS

#endif// AWSMOCK_DB_ENTITY_SNS_MESSAGE_H
