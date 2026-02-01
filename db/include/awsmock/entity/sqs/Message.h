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
    struct Message /*final : Common::BaseEntity<Message>*/ {

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
        long retries = 0;

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
         * MD5 sum of SQS message attributes
         */
        std::string md5MessageAttributes;

        /**
         * MD5 sum sqs system attributes
         */
        std::string md5MessageSystemAttributes;

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
         * These are the user-contributed message attributes.
         */
        std::map<std::string, MessageAttribute> messageAttributes;

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
         * @brief Checks for the existence of an attribute with the given key.
         *
         * @param key attribute key
         * @return true if an attribute with the given key exists
         */
        bool HasAttribute(const std::string &key);

        /**
         * @brief Returns the given attribute as integer
         *
         * @param key attribute key
         * @return true if the attribute with the given key exists
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
        /*
      private:

        friend Message tag_invoke(boost::json::value_to_tag<Message>, boost::json::value const &v) {
            Message r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.oid = v.at("oid").as_string();
            r.queueName = v.at("queueName").as_string();
            r.queueArn = v.at("queueArn").as_string();
            r.body = v.at("body").as_string();
            r.status = MessageStatusFromString(v.at("status").as_string().data());
            r.reset = Core::DateTimeUtils::FromISO8601(v.at("reset").as_string().data());
            r.retries = v.at("retries").as_int64();
            r.size = v.at("score").as_int64();
            r.visibilityTimeout = v.at("visibilityTimeout").as_int64();
            r.messageId = v.at("messageId").as_bool();
            r.receiptHandle = v.at("receiptHandle").as_string();
            r.md5Body = v.at("md5Body").as_string();
            r.md5MessageAttributes = v.at("md5MessageAttributes").as_string();
            r.md5MessageSystemAttributes = v.at("md5MessageSystemAttributes").as_string();
            r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("attributes"));
            r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("messageAttributes"));
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Message const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"oid", obj.oid},
                    {"queueName", obj.queueName},
                    {"queueArn", obj.queueArn},
                    {"body", obj.body},
                    {"status", MessageStatusToString(obj.status)},
                    {"reset", Core::DateTimeUtils::ToISO8601(obj.reset)},
                    {"retries", obj.retries},
                    {"size", obj.size},
                    {"visibilityTimeout", obj.visibilityTimeout},
                    {"messageId", obj.messageId},
                    {"receiptHandle", obj.receiptHandle},
                    {"md5Body", obj.md5Body},
                    {"md5MessageAttributes", obj.md5MessageAttributes},
                    {"md5MessageSystemAttributes", obj.md5MessageSystemAttributes},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"messageAttributes", boost::json::value_from(obj.messageAttributes)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }*/
    };

    typedef std::vector<Message> MessageList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_H
