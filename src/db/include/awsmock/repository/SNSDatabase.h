//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SNS_DATABASE_H
#define AWSMOCK_REPOSITORY_SNS_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/SortColumn.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/memorydb/SNSMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::stream::document;
    using std::chrono::system_clock;

    /**
     * @brief SNS MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit SNSDatabase();

        /**
         * Singleton instance
         */
        static SNSDatabase &instance() {
            static SNSDatabase snsDatabase;
            return snsDatabase;
        }

        /**
         * @brief Check existence of topic
         *
         * @param region AWS region
         * @param topicName topic name
         * @return true if topic already exists
         * @throws DatabaseException
         */
        bool TopicExists(const std::string &region, const std::string &topicName) const;

        /**
         * @brief Check existence of topic
         *
         * @param topicArn topic ARN
         * @return true if topic already exists
         * @throws DatabaseException
         */
        bool TopicExists(const std::string &topicArn) const;

        /**
         * @brief Create a new topic in the SNS topic table
         *
         * @param topic topic entity
         * @return created SNS topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic CreateTopic(Entity::SNS::Topic &topic) const;

        /**
         * @brief Returns a topic by primary key
         *
         * @param oid topic primary key
         * @return topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic GetTopicById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a topic by primary key
         *
         * @param oid topic primary key
         * @return topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic GetTopicById(const std::string &oid) const;

        /**
         * @brief Returns a topic by is ARN
         *
         * @param topicArn topic ARN
         * @return topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic GetTopicByArn(const std::string &topicArn) const;

        /**
         * @brief Returns a topic by its region and name
         *
         * @param region AWS region
         * @param topicName topic name
         * @return topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic GetTopicByName(const std::string &region, const std::string &topicName) const;

        /**
         * @brief Return a topic by target ARN
         *
         * @param targetArn target ARN
         * @return topic with given target ARN
         */
        Entity::SNS::Topic GetTopicByTargetArn(const std::string &targetArn) const;

        /**
         * @brief Return a list of topics with the given subscription ARN
         *
         * @param subscriptionArn subscription ARN
         * @return topic with given topic ARN
         */
        Entity::SNS::TopicList GetTopicsBySubscriptionArn(const std::string &subscriptionArn) const;

        /**
         * @brief Updates an existing topic in the SNS topic table
         *
         * @param topic topic entity
         * @return updated SNS topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic UpdateTopic(Entity::SNS::Topic &topic) const;

        /**
         * @brief Create a new topic or updates an existing topic
         *
         * @param topic topic entity
         * @return created or updated SNS topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic CreateOrUpdateTopic(Entity::SNS::Topic &topic) const;

        /**
         * @brief List all available topics
         *
         * @param region AWS region
         * @return list of SNS topics
         * @throws DatabaseException
         */
        Entity::SNS::TopicList ListTopics(const std::string &region = {}) const;

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
        Entity::SNS::TopicList ListTopics(const std::string &prefix, int pageSize, int pageIndex, const std::vector<Core::SortColumn> &sortColumns, const std::string &region = {}) const;

        /**
         * @brief Counts the number of topics
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of topics
         */
        long CountTopics(const std::string &region = {}, const std::string &prefix = {}) const;

        /**
         * @brief Purge a topic.
         *
         * @param topic topic entity
         * @return total number of deleted messages
         * @throws DatabaseException
         */
        long PurgeTopic(const Entity::SNS::Topic &topic) const;

        /**
         * @brief Calculates the total size of all messages in the topic
         *
         * @param topicArn AWS topic ARN
         * @return total size of the topic
         */
        long GetTopicSize(const std::string &topicArn) const;

        /**
         * @brief Deletes a topic.
         *
         * @param topic topic entity
         * @throws DatabaseException
         */
        void DeleteTopic(const Entity::SNS::Topic &topic) const;

        /**
         * @brief Deletes all topics
         *
         * @return total number of deleted objects
         */
        long DeleteAllTopics() const;

        /**
         * @brief Check existence of message
         *
         * @param messageId message ID
         * @return true if message already exists
         * @throws DatabaseException
         */
        bool MessageExists(const std::string &messageId) const;

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        Entity::SNS::Message CreateMessage(Entity::SNS::Message &message) const;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        Entity::SNS::Message GetMessageById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[maybe_unused]] Entity::SNS::Message GetMessageById(const std::string &oid) const;

        /**
         * @brief Count the number of message by ARN
         *
         * @param topicArn URL of the topic
         * @return number of available messages
         */
        long CountMessages(const std::string &topicArn = {}) const;

        /**
         * @brief  Count the number of message by state
         *
         * @param topicArn ARN of the queue
         * @return total message size
         */
        long CountMessagesSize(const std::string &topicArn = {}) const;

        /**
         * @brief Count the number of message by state
         *
         * @param region AWS region
         * @param topicArn ARN of the topic
         * @param status message status
         */
        long CountMessagesByStatus(const std::string &region, const std::string &topicArn, Entity::SNS::MessageStatus status) const;

        /**
         * @brief Paged list all available messages
         *
         * @param region AWS region
         * @param topicArn AWS topic ARN
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns vector of sort columns
         * @return list of SNS messages
         * @throws DatabaseException
         */
        Entity::SNS::MessageList ListMessages(const std::string &region = {}, const std::string &topicArn = {}, int pageSize = 0, int pageIndex = 0, const std::vector<Core::SortColumn> &sortColumns = {}) const;

        /**
         * @brief Updates an existing message
         *
         * @param message message entity
         * @return created or updated SNS message entity
         * @throws DatabaseException
         */
        Entity::SNS::Message UpdateMessage(Entity::SNS::Message &message) const;

        /**
         * @brief Create a new queue or updates an existing message
         *
         * @param message message entity
         * @return created or updated SNS message entity
         * @throws DatabaseException
         */
        Entity::SNS::Message CreateOrUpdateMessage(Entity::SNS::Message &message) const;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @throws Core::DatabaseException
         */
        void DeleteMessage(const Entity::SNS::Message &message) const;

        /**
         * @brief Deletes a message by message ID.
         *
         * @param messageId message ID to delete
         * @throws Core::DatabaseException
         */
        void DeleteMessage(const std::string &messageId) const;

        /**
         * @brief Bulk delete of resources.
         *
         * @param region AWS region
         * @param topicArn topic ARN
         * @param receipts vector of receipts
         * @throws Core::DatabaseException
         */
        void DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &receipts) const;

        /**
         * @brief Deletes old resources message.
         *
         * @param timeout timeout in seconds
         * @throws Core::DatabaseException
         */
        void DeleteOldMessages(long timeout) const;

        /**
         * @brief Deletes a resources.
         *
         * @throws Core::DatabaseException
         */
        void DeleteAllMessages() const;

        /**
         * @brief Adjust all message counters
         */
        void AdjustAllMessageCounters() const;

        /**
         * @brief Adjust message counters for a single topic
         *
         * @param topicArn AWS topic ARN
         */
        void AdjustMessageCounters(const std::string &topicArn) const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Topic collection name
         */
        std::string _topicCollectionName;

        /**
         * Message collection name
         */
        std::string _messageCollectionName;

        /**
         * SNS in-memory database
         */
        SNSMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SNS_DATABASE_H
