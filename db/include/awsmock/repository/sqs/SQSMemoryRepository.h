//
// Created by vogje01 on 11/19/23.
//

#pragma once

// C++ includes
#include <string>

// Boost includes
#include <boost/container/map.hpp>
#include <boost/container/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include "ISQSRepository.h"


#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/Linq.h>
#include <awsmock/core/PagingUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/MessageWaitTime.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    using std::chrono::system_clock;

    /**
     * @brief SQS in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSMemoryRepository final : public ISQSRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit SQSMemoryRepository() = default;

        /**
         * @brief Singleton instance
         */
        static SQSMemoryRepository &instance() {
            static SQSMemoryRepository sqsMemoryDb;
            return sqsMemoryDb;
        }

        /**
         * @brief Check the existence of the queue
         *
         * @param region AWS region
         * @param name queue name
         * @return true if a queue already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool queueExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Check the existence of the queue
         *
         * @param region AWS region
         * @param queueUrl AWS region
         * @return true if a queue already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool queueUrlExists(const std::string &region, const std::string &queueUrl) const override;

        /**
         * @brief Check the existence of queue by ARN
         *
         * @param queueArn queue ARN
         * @return true in case the queue exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool queueArnExists(const std::string &queueArn) const override;

        /**
         * @brief Create a new queue in the SQS queue table
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue createQueue(Entity::SQS::Queue &queue) const override;

        /**
         * @brief Create a new queue or update an existing in the SQS queue table
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue createOrUpdateQueue(Entity::SQS::Queue &queue) const override;

        /**
         * @brief Returns a queue by primary key
         *
         * @param oid queue primary key
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue getQueueById(const std::string &oid) const override;

        /**
         * @brief Returns a queue by primary key
         *
         * @param oid queue primary key
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue getQueueById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Get a paged and sorted list of all available queues
         *
         * @param prefix queue name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sort columns
         * @param region AWS region
         * @return list of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::QueueList listQueues(const std::string &prefix = {}, long pageSize = 0, long pageIndex = 0, const std::vector<SortColumn> &sortColumns = {}, const std::string &region = {}) const override;

        /**
         * @brief Get a paged and sorted list of all available queues
         *
         * @param region AWS region
         * @return list of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::QueueList listQueues(const std::string &region) const override;

        /**
         * @brief List available queues using paging
         *
         * @param sortColumns vector of sort columns
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::QueueList exportQueues(const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Import s a single queue
         *
         * @par
         * During import of a queue, the queue URL is adjusted, as it contains the hostname of the machine where the
         * queue is imported. Additionally, the counters are set to zero, and the modified timestamp is adjusted.
         *
         * @param queue queue entity
         * @throws DatabaseException
         */
        void importQueue(Entity::SQS::Queue &queue) const override;

        /**
         * @brief Returns a queue by ARN
         *
         * @param queueArn queue ARN
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue getQueueByArn(const std::string &queueArn) const override;

        /**
         * @brief Returns a queue by name and region
         *
         * @param region AWS region
         * @param queueName queue name
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue getQueueByName(const std::string &region, const std::string &queueName) const override;

        /**
         * @brief Returns a queue by URL
         *
         * @param region AWS region.
         * @param queueUrl queue URL.
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue getQueueByUrl(const std::string &region, const std::string &queueUrl) const override;

        /**
         * @brief Returns a queue by DQL ARN
         *
         * @param dlqQueueArn queue ARN of the DQL
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue getQueueByDlq(const std::string &dlqQueueArn) const override;

        /**
         * @brief Checks whether a queue ARN is a dead letter queue
         *
         * @param queueArn queue ARN
         * @return list of queues
         */
        [[nodiscard]]
        std::vector<Entity::SQS::Queue> isDlq(const std::string &queueArn) const override;

        /**
         * @brief Purge a given queueUrl.
         *
         * @param queueArn queue ARN
         * @return total number of deleted messages
         */
        long purgeQueue(const std::string &queueArn) const override;

        /**
         * @brief Updates a given queue.
         *
         * @param queue AWS region
         * @return updated queue
         */
        Entity::SQS::Queue updateQueue(Entity::SQS::Queue &queue) const override;

        /**
         * @brief Count the number of queues for a given region.
         *
         * @param region AWS region
         * @param prefix queue name prefix
         * @return number of queues in the given region.
         */
        long countQueues(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Deletes a queue.
         *
         * @param queue queue entity
         * @return number of deleted queues
         * @throws DatabaseException
         */
        long deleteQueue(const Entity::SQS::Queue &queue) const override;

        /**
         * @brief Deletes all queues
         *
         * @return number of deleted queues
         * @throws DatabaseException
         */
        long deleteAllQueues() const override;

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message createMessage(Entity::SQS::Message &message) const override;

        /**
         * @brief Creates a new message or updated an existing message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message createOrUpdateMessage(Entity::SQS::Message &message) const override;

        /**
         * @brief Checks whether the message exists by the receipt handle.
         *
         * @param receiptHandle receipt handle
         * @return true, if the message exists, otherwise false
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        bool messageExists(const std::string &receiptHandle) const override;

        /**
         * @brief Checks whether the message exists by message ID
         *
         * @param messageId SQS message ID
         * @return true if message exists, otherwise false
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        bool messageExistsByMessageId(const std::string &messageId) const override;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message getMessageById(const std::string &oid) const override;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message getMessageById(bsoncxx::oid oid) const override;

        /**
         * @brief Updates a given message.
         *
         * @param message SQS message
         * @return updated message
         */
        Entity::SQS::Message updateMessage(Entity::SQS::Message &message) const override;

        /**
         * @brief List all available resources
         *
         * @param region AWS region
         * @return list of SQS resources
         * @throws DatabaseException
         */
        Entity::SQS::MessageList listMessages(const std::string &region) const override;

        /**
         * @brief Retrieves a list of messages.
         *
         * @param queueArn queue AWS ARN
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return A list of messages that match the given filter criteria.
         */
        Entity::SQS::MessageList listMessages(const std::string &queueArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Receive messages from a queue.
         *
         * @param queueArn queue ARN
         * @param visibility in seconds
         * @param maxResult maximal message count to receive
         * @param dlQueueArn dead letter queue ARN
         * @param maxRetries maximal number of retries
         * @param messageList message list
         */
        void receiveMessages(const std::string &queueArn, long visibility, long maxResult, const std::string &dlQueueArn, long maxRetries, Entity::SQS::MessageList &messageList) const override;

        /**
         * @brief Reset expired resources
         *
         * @param queueArn ARN of the queue
         * @param visibility visibilityTimeout period in seconds
         * @return number of message resets
         */
        long resetMessages(const std::string &queueArn, long visibility) const override;

        /**
         * @brief Any message which has a message state is DELAYED is reset when the delay period is over.
         *
         * @param queueArn queue URL.
         * @param delay message delay in seconds
         * @return number of delayed messages
         */
        long resetDelayedMessages(const std::string &queueArn, long delay) const override;

        /**
         * @brief Redrive a single message
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @param messageId message ID
         */
        long redriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId) const override;

        /**
         * @brief Redrive messages
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @return total number of redriven messages
         */
        long redriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const override;

        /**
         * @brief Any message, which has is older than the retention period is deleted.
         *
         * @param queueArn queue URL.
         * @param retentionPeriod retention period in seconds.
         * @return number of deleted messages
         */
        long messageRetention(const std::string &queueArn, long retentionPeriod) const override;

        /**
          * @brief Returns a message by receipt handle.
          *
          * @param receiptHandle message receipt handle
          * @return message entity
          * @throws Core::DatabaseException
          */
        Entity::SQS::Message getMessageByReceiptHandle(const std::string &receiptHandle) const override;

        /**
         * @brief Returns a message by message ID
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message getMessageByMessageId(const std::string &messageId) const override;

        /**
         * @brief Count the number of messages by queue
         *
         * @param queueArn AWS queue ARN
         * @param prefix message prefix
         */
        long countMessages(const std::string &queueArn, const std::string &prefix) const override;

        /**
         * @brief Returns the average waiting time for messages in the given queue
         *
         * @par
         * Uses a simple min, max query to get the first and the last entry in the sqs_message collection. The average is then calculated as
         * (max-min)/2. This is done on a per queue arn basis.
         *
         * @return map of average message waiting time per queue
         * @throws Core::DatabaseException
         */
        Entity::SQS::MessageWaitTime getAverageMessageWaitingTime() const override;

        /**
         * @brief Import messages via bulk updates
         *
         * @param queueArn queue ARN
         * @param messageArray
         */
        void importMessages(const std::string &queueArn, const value &messageArray) const override;


        /**
         * @brief Deletes all resources of a queue
         *
         * @param queueArn message queue to delete resources from
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        long deleteMessages(const std::string &queueArn) const override;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        long deleteMessage(const Entity::SQS::Message &message) const override;

        /**
         * @brief Deletes a message by receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        long deleteMessage(const std::string &receiptHandle) const override;

        /**
         * @brief Deletes resources.
         *
         * @return total number of messages deleted
         * @throws Core::DatabaseException
         */
        long deleteAllMessages() const override;

        /**
         * @brief Adjust all queue counters
         */
        void adjustMessageCounters() const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SQS"};

        /**
         * @brief Return the total number of message for a queue with the given status
         *
         * @param queueArn queue AWS ARN
         * @param status status
         * @return number of messages with the given status
         */
        long countMessagesByStatus(const std::string &queueArn, const Entity::SQS::MessageStatus &status) const;

        /**
         * SQS queue map when running without a database
         */
        mutable std::unordered_map<std::string, Entity::SQS::Queue> _queues{};

        /**
         * SQS message map when running without a database
         */
        mutable std::unordered_map<std::string, Entity::SQS::Message> _messages{};

        /**
         * Queue mutex
         */
        static boost::mutex _sqsQueueMutex;

        /**
         * Message mutex
         */
        static boost::mutex _sqsMessageMutex;
    };

}// namespace Awsmock::Database
