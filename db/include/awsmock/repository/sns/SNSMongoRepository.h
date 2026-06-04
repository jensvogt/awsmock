//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SNS_DATABASE_H
#define AWSMOCK_REPOSITORY_SNS_DATABASE_H

// C++ standard includes
#include <set>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/MessageStatus.h>
#include <awsmock/entity/sns/Topic.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/DatabaseBase.h>
#include <awsmock/repository/sns/ISNSRepository.h>
#include <awsmock/utils/SortColumn.h>
#include <awsmock/utils/SqsUtils.h>

namespace Awsmock::Database {

    using std::chrono::system_clock;

    /**
     * @brief SNS MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSMongoRepository final : public ISNSRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit SNSMongoRepository() = default;

        /**
         * @brief Check the existence of a topic
         *
         * @param region AWS region
         * @param topicName topic name
         * @return true if the topic already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool topicExists(const std::string &region, const std::string &topicName) const override;

        /**
         * @brief Check the existence of a topic
         *
         * @param topicArn topic ARN
         * @return true if the topic already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool topicExists(const std::string &topicArn) const override;

        /**
         * @brief Create a new topic in the SNS topic table
         *
         * @param topic topic entity
         * @return created SNS topic entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Topic createTopic(Entity::SNS::Topic &topic) const override;

        /**
         * @brief Returns a topic by primary key
         *
         * @param oid topic primary key
         * @return topic entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Topic getTopicById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a topic by primary key
         *
         * @param oid topic primary key
         * @return topic entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Topic getTopicById(const std::string &oid) const override;

        /**
         * @brief Returns a topic by is ARN
         *
         * @param topicArn topic ARN
         * @return topic entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Topic getTopicByArn(const std::string &topicArn) const override;

        /**
         * @brief Returns a topic by its region and name
         *
         * @param region AWS region
         * @param topicName topic name
         * @return topic entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Topic getTopicByName(const std::string &region, const std::string &topicName) const override;

        /**
         * @brief Return a topic by target ARN
         *
         * @param targetArn target ARN
         * @return topic with given target ARN
         */
        [[nodiscard]]
        Entity::SNS::Topic getTopicByTargetArn(const std::string &targetArn) const override;

        /**
         * @brief Return a list of topics with the given subscription ARN
         *
         * @param subscriptionArn subscription ARN
         * @return topic with given topic ARN
         */
        [[nodiscard]]
        Entity::SNS::TopicList getTopicsBySubscriptionArn(const std::string &subscriptionArn) const override;

        /**
         * @brief Updates an existing topic in the SNS topic table
         *
         * @param topic topic entity
         * @return updated SNS topic entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Topic updateTopic(Entity::SNS::Topic &topic) const override;

        /**
         * @brief Create a new topic or updates an existing topic
         *
         * @param topic topic entity
         * @return created or updated SNS topic entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Topic createOrUpdateTopic(Entity::SNS::Topic &topic) const override;

        /**
         * @brief Imports a topic
         *
         * @param topic topic entity
         * @throws DatabaseException
         */
        void importTopic(Entity::SNS::Topic &topic) const override;

        /**
         * @brief List all available topics
         *
         * @param region AWS region
         * @return list of SNS topics
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::TopicList listTopics(const std::string &region) const override;

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
        [[nodiscard]]
        Entity::SNS::TopicList listTopics(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const override;

        /**
         * @brief Export all available topics
         *
         * @param sortColumns sort columns
         * @return list of SNS topics
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::TopicList exportTopics(const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Counts the number of topics
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return number of topics
         */
        [[nodiscard]]
        long countTopics(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Purge a topic.
         *
         * @param topic topic entity
         * @return total number of deleted messages
         * @throws DatabaseException
         */
        [[nodiscard]]
        long purgeTopic(const Entity::SNS::Topic &topic) const override;

        /**
         * @brief Calculates the total size of all messages in the topic
         *
         * @param topicArn AWS topic ARN
         * @return total size of the topic
         */
        [[nodiscard]]
        long getTopicSize(const std::string &topicArn) const override;

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
        void updateTopicCounter(const std::string &topicArn, long messages, long size, long initial, long send, long resend) const override;

        /**
         * @brief Deletes a topic.
         *
         * @param topic topic entity
         * @throws DatabaseException
         */
        void deleteTopic(const Entity::SNS::Topic &topic) const override;

        /**
         * @brief Deletes all topics
         *
         * @return total number of deleted objects
         */
        [[nodiscard]]
        long deleteAllTopics() const override;

        /**
         * @brief Check the existence of a message
         *
         * @param messageId message ID
         * @return true if the message already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool messageExists(const std::string &messageId) const override;

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Message createMessage(Entity::SNS::Message &message) const override;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Message getMessageById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Message getMessageById(const std::string &oid) const override;

        /**
         * @brief Returns a message by message ID.
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Message getMessageByMessageId(const std::string &messageId) const override;

        /**
         * @brief Count the number of messages by ARN
         *
         * @param topicArn URL of the topic
         * @return number of available messages
         */
        [[nodiscard]]
        long countMessages(const std::string &topicArn) const override;

        /**
         * @brief Count the number of messages by state
         *
         * @param topicArn ARN of the topic
         * @param status message status
         */
        [[nodiscard]]
        long countMessagesByStatus(const std::string &topicArn, Entity::SNS::MessageStatus status) const override;

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
        [[nodiscard]]
        Entity::SNS::MessageList listMessages(const std::string &topicArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Updates an existing message
         *
         * @param message message entity
         * @return created or updated SNS message entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Message updateMessage(Entity::SNS::Message &message) const override;

        /**
         * @brief Create a new queue or updates an existing message
         *
         * @param message message entity
         * @return created or updated SNS message entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SNS::Message createOrUpdateMessage(Entity::SNS::Message &message) const override;

        /**
         * @brief Sets the message status
         *
         * @param message SNS message
         * @param status new status
         */
        void setMessageStatus(const Entity::SNS::Message &message, const Entity::SNS::MessageStatus &status) const override;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteMessage(const Entity::SNS::Message &message) const override;

        /**
         * @brief Deletes a message by message ID.
         *
         * @param messageId message ID to delete
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteMessage(const std::string &messageId) const override;

        /**
         * @brief Bulk delete of resources.
         *
         * @param region AWS region
         * @param topicArn topic ARN
         * @param messageIds vector of receipts
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) const override;

        /**
         * @brief Deletes old messages.
         *
         * @param timeout timeout in seconds
         * @throws Core::DatabaseException
         */
        void deleteOldMessages(long timeout) const override;

        /**
         * @brief Deletes a message.
         *
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteAllMessages() const override;

        /**
         * @brief Adjust all topic counters
         */
        void adjustMessageCounters() const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SNS"};

        /**
         * Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * Topic collection name
         */
        static constexpr auto _topicCollectionName = "sns_topic";

        /**
         * Message collection name
         */
        static constexpr auto _messageCollectionName = "sns_message";
    };

}// namespace Awsmock::Database

#endif// AWSMOCK_REPOSITORY_SNS_DATABASE_H
