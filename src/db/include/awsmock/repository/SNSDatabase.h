//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SNS_DATABASE_H
#define AWSMOCK_REPOSITORY_SNS_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// Boost includes
#include <boost/container/map.hpp>
#include <boost/container/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringCollector.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/memorydb/SNSMemoryDb.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    using std::chrono::system_clock;

    struct TopicMonitoringCounter {
        long initial{};
        long send{};
        long resend{};
        long messages{};
        long size{};
        system_clock::time_point modified = system_clock::now();
    };

    using SnsShmAllocator = boost::interprocess::allocator<std::pair<const std::string, TopicMonitoringCounter>, boost::interprocess::managed_shared_memory::segment_manager>;
    using SnsCounterMapType = boost::container::map<std::string, TopicMonitoringCounter, std::less<std::string>, SnsShmAllocator>;

    static constexpr auto SNS_COUNTER_MAP_NAME = "SnsBucketCounter";

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
         * @brief Check the existence of a topic
         *
         * @param region AWS region
         * @param topicName topic name
         * @return true if the topic already exists
         * @throws DatabaseException
         */
        bool TopicExists(const std::string &region, const std::string &topicName) const;

        /**
         * @brief Check the existence of a topic
         *
         * @param topicArn topic ARN
         * @return true if the topic already exists
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
         * @brief Imports a topic
         *
         * @param topic topic entity
         * @throws DatabaseException
         */
        void ImportTopic(Entity::SNS::Topic &topic) const;

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
        Entity::SNS::TopicList ListTopics(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region = {}) const;

        /**
         * @brief Export all available topics
         *
         * @param sortColumns sort columns
         * @return list of SNS topics
         * @throws DatabaseException
         */
        Entity::SNS::TopicList ExportTopics(const std::vector<SortColumn> &sortColumns) const;

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
        [[nodiscard]] long GetTopicSize(const std::string &topicArn) const;

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
        void UpdateTopicCounter(const std::string &topicArn, long messages, long size, long initial, long send, long resend) const;

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
         * @brief Check the existence of message
         *
         * @param messageId message ID
         * @return true if the message already exists
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
         * @brief Returns a message by message ID.
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[maybe_unused]] Entity::SNS::Message GetMessageByMessageId(const std::string &messageId) const;

        /**
         * @brief Count the number of messages by ARN
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
         * @brief Count the number of messages by state
         *
         * @param topicArn ARN of the topic
         * @param status message status
         */
        long CountMessagesByStatus(const std::string &topicArn, Entity::SNS::MessageStatus status) const;

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
        [[nodiscard]] Entity::SNS::MessageList ListMessages(const std::string &region = {}, const std::string &topicArn = {}, long pageSize = 0, long pageIndex = 0, const std::vector<SortColumn> &sortColumns = {}) const;

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
         * @brief Sets the message status
         *
         * @param message SNS message
         * @param status new status
         */
        void SetMessageStatus(const Entity::SNS::Message &message, const Entity::SNS::MessageStatus &status) const;

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
         * @param messageIds vector of receipts
         * @throws Core::DatabaseException
         */
        void DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) const;

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
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        long DeleteAllMessages() const;

        /**
         * @brief Adjust all topic counters
         */
        void AdjustMessageCounters() const;

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
