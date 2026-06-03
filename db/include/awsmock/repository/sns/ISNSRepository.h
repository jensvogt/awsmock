//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for SQS repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * SQS-related data.
     */
    class ISNSRepository {

      public:

        /**
         * @brief Virtual destructor for the ISQSRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~ISNSRepository() = default;

        /**
         * @brief Check the existence of a topic
         *
         * @param region AWS region
         * @param topicName topic name
         * @return true if the topic already exists
         * @throws DatabaseException
         */
        virtual bool topicExists(const std::string &region, const std::string &topicName) const = 0;

        /**
         * @brief Check the existence of a topic
         *
         * @param topicArn topic ARN
         * @return true if the topic already exists
         * @throws DatabaseException
         */
        virtual bool topicExists(const std::string &topicArn) const = 0;

        /**
         * @brief Create a new topic in the SNS topic table
         *
         * @param topic topic entity
         * @return created SNS topic entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Topic createTopic(Entity::SNS::Topic &topic) const = 0;

        /**
         * @brief Returns a topic by primary key
         *
         * @param oid topic primary key
         * @return topic entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Topic getTopicById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Returns a topic by primary key
         *
         * @param oid topic primary key
         * @return topic entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Topic getTopicById(const std::string &oid) const = 0;

        /**
         * @brief Returns a topic by is ARN
         *
         * @param topicArn topic ARN
         * @return topic entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Topic getTopicByArn(const std::string &topicArn) const = 0;

        /**
         * @brief Returns a topic by its region and name
         *
         * @param region AWS region
         * @param topicName topic name
         * @return topic entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Topic getTopicByName(const std::string &region, const std::string &topicName) const = 0;

        /**
         * @brief Return a topic by target ARN
         *
         * @param targetArn target ARN
         * @return topic with given target ARN
         */
        virtual Entity::SNS::Topic getTopicByTargetArn(const std::string &targetArn) const = 0;

        /**
         * @brief Return a list of topics with the given subscription ARN
         *
         * @param subscriptionArn subscription ARN
         * @return topic with given topic ARN
         */
        virtual Entity::SNS::TopicList getTopicsBySubscriptionArn(const std::string &subscriptionArn) const = 0;

        /**
         * @brief Updates an existing topic in the SNS topic table
         *
         * @param topic topic entity
         * @return updated SNS topic entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Topic updateTopic(Entity::SNS::Topic &topic) const = 0;

        /**
         * @brief Create a new topic or updates an existing topic
         *
         * @param topic topic entity
         * @return created or updated SNS topic entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Topic createOrUpdateTopic(Entity::SNS::Topic &topic) const = 0;

        /**
         * @brief Imports a topic
         *
         * @param topic topic entity
         * @throws DatabaseException
         */
        virtual void importTopic(Entity::SNS::Topic &topic) const = 0;

        /**
         * @brief List all available topics
         *
         * @param region AWS region
         * @return list of SNS topics
         * @throws DatabaseException
         */
        virtual Entity::SNS::TopicList listTopics(const std::string &region) const = 0;

        /**
         * @brief List all available topics
         *
         * @param region AWS region
         * @param prefix queue name prefix
         * @param pageSize maximal number of results
         * @param pageIndex pge index
         * @param sortColumns vector of sort columns
         * @return list of SNS topics
         * @throws DatabaseException
         */
        virtual Entity::SNS::TopicList listTopics(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const = 0;

        /**
         * @brief Export all available topics
         *
         * @param sortColumns sort columns
         * @return list of SNS topics
         * @throws DatabaseException
         */
        virtual Entity::SNS::TopicList exportTopics(const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Counts the number of topics
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of topics
         */
        virtual long countTopics(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Purge a topic.
         *
         * @param topic topic entity
         * @return total number of deleted messages
         * @throws DatabaseException
         */
        virtual long purgeTopic(const Entity::SNS::Topic &topic) const = 0;

        /**
         * @brief Calculates the total size of all messages in the topic
         *
         * @param topicArn AWS topic ARN
         * @return total size of the topic
         */
        virtual long getTopicSize(const std::string &topicArn) const = 0;

        /**
         * @brief Updates the counters of a topic
         *
         * @param topicArn topic ARN
         * @param messages number of keys
         * @param size bucket size
         * @param initial messages in status INITIAL
         * @param send messages in status SEND
         * @param resend messages in status RESEND
         * @return created bucket entity
         * @throws DatabaseException
         */
        virtual void updateTopicCounter(const std::string &topicArn, long messages, long size, long initial, long send, long resend) const = 0;

        /**
         * @brief Deletes a topic.
         *
         * @param topic topic entity
         * @throws DatabaseException
         */
        virtual void deleteTopic(const Entity::SNS::Topic &topic) const = 0;

        /**
         * @brief Deletes all topics
         *
         * @return total number of deleted objects
         */
        virtual long deleteAllTopics() const = 0;

        /**
         * @brief Check the existence of the message
         *
         * @param messageId message ID
         * @return true if the message already exists
         * @throws DatabaseException
         */
        virtual bool messageExists(const std::string &messageId) const = 0;

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        virtual Entity::SNS::Message createMessage(Entity::SNS::Message &message) const = 0;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        virtual Entity::SNS::Message getMessageById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        virtual Entity::SNS::Message getMessageById(const std::string &oid) const = 0;

        /**
         * @brief Returns a message by message ID.
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        virtual Entity::SNS::Message getMessageByMessageId(const std::string &messageId) const = 0;

        /**
         * @brief Count the number of messages by ARN
         *
         * @param topicArn URL of the topic
         * @return number of available messages
         */
        virtual long countMessages(const std::string &topicArn) const = 0;
        
        /**
         * @brief Count the number of messages by state
         *
         * @param topicArn ARN of the topic
         * @param status message status
         */
        virtual long countMessagesByStatus(const std::string &topicArn, Entity::SNS::MessageStatus status) const = 0;

        /**
         * @brief Paged list all available messages
         *
         * @param topicArn AWS topic ARN
         * @param prefix message ID prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns vector of sort columns
         * @return list of SNS messages
         * @throws DatabaseException
         */
        virtual Entity::SNS::MessageList listMessages(const std::string &topicArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Updates an existing message
         *
         * @param message message entity
         * @return created or updated SNS message entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Message updateMessage(Entity::SNS::Message &message) const = 0;

        /**
         * @brief Create a new queue or updates an existing message
         *
         * @param message message entity
         * @return created or updated SNS message entity
         * @throws DatabaseException
         */
        virtual Entity::SNS::Message createOrUpdateMessage(Entity::SNS::Message &message) const = 0;

        /**
         * @brief Sets the message status
         *
         * @param message SNS message
         * @param status new status
         */
        virtual void setMessageStatus(const Entity::SNS::Message &message, const Entity::SNS::MessageStatus &status) const = 0;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @throws Core::DatabaseException
         */
        virtual long deleteMessage(const Entity::SNS::Message &message) const = 0;

        /**
         * @brief Deletes a message by message ID.
         *
         * @param messageId message ID to delete
         * @throws Core::DatabaseException
         */
        virtual long deleteMessage(const std::string &messageId) const = 0;

        /**
         * @brief Bulk delete of resources.
         *
         * @param region AWS region
         * @param topicArn topic ARN
         * @param messageIds vector of receipts
         * @throws Core::DatabaseException
         */
        virtual long deleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) const = 0;

        /**
         * @brief Deletes an old messages.
         *
         * @param timeout timeout in seconds
         * @throws Core::DatabaseException
         */
        virtual void deleteOldMessages(long timeout) const = 0;

        /**
         * @brief Deletes a message.
         *
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        virtual long deleteAllMessages() const = 0;

        /**
         * @brief Adjust all topic counters
         */
        virtual void adjustMessageCounters() const = 0;
    };

}// namespace Awsmock::Database