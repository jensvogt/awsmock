//
// Created by vogje01 on 11/19/23.
//

#ifndef AWSMOCK_REPOSITORY_SNS_MEMORYDB_H
#define AWSMOCK_REPOSITORY_SNS_MEMORYDB_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/entity/sns/Message.h>
#include <awsmock/entity/sns/Topic.h>
#include <boost/thread/pthread/mutex.hpp>

namespace AwsMock::Database {

    using std::chrono::system_clock;

    /**
     * SNS in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SNSMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        SNSMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static SNSMemoryDb &instance() {
            static SNSMemoryDb snsMemoryDb;
            return snsMemoryDb;
        }

        /**
         * @brief Check existence of topic
         *
         * @param region AWS region
         * @param name topic name
         * @return true if topic already exists
         * @throws DatabaseException
         */
        bool TopicExists(const std::string &region, const std::string &name);

        /**
         * @brief Check existence of topic
         *
         * @param topicName topic ARN
         * @return true if topic already exists
         * @throws DatabaseException
         */
        bool TopicExists(const std::string &topicName);

        /**
         * @brief Create a new topic in the SNS topic table
         *
         * @param topic topic entity
         * @return created SNS topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic CreateTopic(const Entity::SNS::Topic &topic);

        /**
         * @brief Returns a topic by primary key
         *
         * @param oid topic primary key
         * @return topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic GetTopicById(const std::string &oid);

        /**
         * @brief Returns a topic by is ARN
         *
         * @param topicArn topic ARN
         * @return topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic GetTopicByArn(const std::string &topicArn);

        /**
         * @brief Returns a topic by its region and name
         *
         * @param region AWS region
         * @param topicName topic name
         * @return topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic GetTopicByName(const std::string &region, const std::string &topicName);

        /**
         * @brief Return a list of topics with the given subscription ARN
         *
         * @param subscriptionArn subscription ARN
         * @return topic with given topic ARN
         */
        Entity::SNS::TopicList GetTopicsBySubscriptionArn(const std::string &subscriptionArn);

        /**
         * @brief Updates an existing topic in the SNS topic table
         *
         * @param topic topic entity
         * @return updated SNS topic entity
         * @throws DatabaseException
         */
        Entity::SNS::Topic UpdateTopic(Entity::SNS::Topic &topic);

        /**
         * @brief List all available topics
         *
         * @param region AWS region
         * @return list of SNS topics
         * @throws DatabaseException
         */
        Entity::SNS::TopicList ListTopics(const std::string &region = {}) const;

        /**
         * @brief Counts the number of topics
         *
         * @param region AWS region
         * @return number of topics
         */
        long CountTopics(const std::string &region = {}) const;

        /**
         * @brief Purge a topic.
         *
         * @param topic topic entity
         * @return total number of deleted messages
         * @throws DatabaseException
         */
        long PurgeTopic(const Entity::SNS::Topic &topic);

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
        void DeleteTopic(const Entity::SNS::Topic &topic);

        /**
         * @brief Deletes all topics
         */
        void DeleteAllTopics();

        /**
         * @brief Check existence of message
         *
         * @param id message ID
         * @return true if message already exists
         * @throws DatabaseException
         */
        bool MessageExists(const std::string &id);

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        Entity::SNS::Message CreateMessage(const Entity::SNS::Message &message);

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[maybe_unused]] Entity::SNS::Message GetMessageById(const std::string &oid);

        /**
         * @brief Count the number of message by ARN
         *
         * @param topicArn ARN of the topic
         * @return number of available messages
         */
        long CountMessages(const std::string &topicArn = {}) const;

        /**
         * @brief List all available resources
         *
         * @param region AWS region
         * @param topicArn AWS region
         * @return list of SNS resources
         * @throws DatabaseException
         */
        Entity::SNS::MessageList ListMessages(const std::string &region = {}, const std::string &topicArn = {});

        /**
         * @brief Updates a given message.
         *
         * @param message SNS message
         * @return updated message
         */
        Entity::SNS::Message UpdateMessage(Entity::SNS::Message &message);

        /**
         * @brief Count the number of message by state
         *
         * @param region AWS region
         * @param topicArn ARN of the topic
         * @param status message status
         */
        long CountMessagesByStatus(const std::string &region, const std::string &topicArn, Entity::SNS::MessageStatus status) const;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @throws Core::DatabaseException
         */
        void DeleteMessage(const Entity::SNS::Message &message);

        /**
         * @brief Deletes a message by message ID.
         *
         * @param messageId message ID to delete
         * @throws Core::DatabaseException
         */
        void DeleteMessage(const std::string &messageId);

        /**
         * @brief Bulk delete of resources.
         *
         * @param region AWS region
         * @param topicArn topic ARN
         * @param receipts vector of receipts
         * @throws Core::DatabaseException
         */
        void DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &receipts);

        /**
          * @brief Deletes old resources message.
          *
          * @param timeout timeout period
          * @throws Core::DatabaseException
          */
        void DeleteOldMessages(long timeout);

        /**
         * @brief Deletes a resources.
         *
         * @throws Core::DatabaseException
         */
        void DeleteAllMessages();

      private:

        /**
         * SNS topic vector, when running without database
         */
        std::map<std::string, Entity::SNS::Topic> _topics{};

        /**
         * SNS message vector, when running without database
         */
        std::map<std::string, Entity::SNS::Message> _messages{};

        /**
         * Topic mutex
         */
        static boost::mutex _snsTopicMutex;

        /**
         * Message mutex
         */
        static boost::mutex _snsMessageMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SNS_MEMORYDB_H
