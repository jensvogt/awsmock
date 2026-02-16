//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_SQS_DATABASE_H
#define AWSMOCK_REPOSITORY_SQS_DATABASE_H

// C++ standard includes
#include <queue>
#include <string>

// Boost includes
#include <boost/container/map.hpp>
#include <boost/container/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringCollector.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/MessageWaitTime.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/memorydb/SQSMemoryDb.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/MongoUtils.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    using std::chrono::system_clock;

    /**
     * @brief SQS MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit SQSDatabase();

        /**
         * @brief Singleton instance
         */
        static SQSDatabase &instance() {
            static SQSDatabase sqsDatabase;
            return sqsDatabase;
        }

        /**
         * @brief Check the existence of a queue
         *
         * @param region AWS region
         * @param name queue name
         * @return true if the queue already exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool QueueExists(const std::string &region, const std::string &name) const;

        /**
         * @brief Check the existence of a queue
         *
         * @param region AWS region
         * @param queueUrl AWS region
         * @return true if the queue already exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool QueueUrlExists(const std::string &region, const std::string &queueUrl) const;

        /**
         * @brief Check the existence of queue by ARN
         *
         * @param queueArn queue ARN
         * @return true in case the queue exists
         * @throws DatabaseException
         */
        [[nodiscard]] bool QueueArnExists(const std::string &queueArn) const;

        /**
         * @brief Create a new queue in the SQS queue table
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue CreateQueue(Entity::SQS::Queue &queue) const;

        /**
         * @brief Returns a queue by primary key
         *
         * @param oid queue primary key
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::Queue GetQueueById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a queue by primary key
         *
         * @param oid queue primary key
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::Queue GetQueueById(const std::string &oid) const;

        /**
         * @brief Returns a queue by ARN
         *
         * @param queueArn queue ARN
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::Queue GetQueueByArn(const std::string &queueArn) const;

        /**
         * @brief Returns a queue by DQL ARN
         *
         * @param dlqQueueArn queue ARN of the DQL
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::Queue GetQueueByDlq(const std::string &dlqQueueArn) const;

        /**
         * @brief Returns a queue by name and region
         *
         * @param region AWS region
         * @param queueName queue name
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::Queue GetQueueByName(const std::string &region, const std::string &queueName) const;

        /**
         * @brief Returns a queue by URL
         *
         * @param region AWS region
         * @param queueUrl queue URL
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::Queue GetQueueByUrl(const std::string &region, const std::string &queueUrl) const;

        /**
         * @brief List available queues, using paging
         *
         * @param prefix queue name prefix
         * @param pageSize maximal number of results
         * @param pageIndex pge index
         * @param sortColumns vector of sort columns
         * @param region AWS region
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::QueueList ListQueues(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region = {}) const;

        /**
         * @brief List available queues, using paging
         *
         * @param sortColumns vector of sort columns
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::QueueList ExportQueues(const std::vector<SortColumn> &sortColumns) const;

        /**
         * @brief Import s a single queue
         *
         * @par
         * During import of a queue, the queue URL is adjusted, as it contains the hostname of the machine where the
         * queue is imported. Additionally, the counters are set to zero and the modified timestamp is adjusted.
         *
         * @param queue queue entity
         * @throws DatabaseException
         */
        void ImportQueue(Entity::SQS::Queue &queue) const;

        /**
         * @brief List all available queues
         *
         * @param region AWS region
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::QueueList ListQueues(const std::string &region = {}) const;

        /**
         * @brief Purge a given queueUrl.
         *
         * @param queueArn queue ARN
         * @return total number of deleted messages
         */
        [[nodiscard]] long PurgeQueue(const std::string &queueArn) const;

        /**
         * @brief Updates a given queue.
         *
         * @param queue AWS region
         * @return updated queue
         */
        Entity::SQS::Queue UpdateQueue(Entity::SQS::Queue &queue) const;

        /**
         * @brief Create a new queue or updates an existing queue
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue CreateOrUpdateQueue(Entity::SQS::Queue &queue) const;

        /**
         * @brief Updates the counters of a queue
         *
         * @param queueArn queue ARN
         * @param messages number of keys
         * @param size bucket size
         * @param initial messages in status INITIAL
         * @param invisible messages in status INVISIBLE
         * @param delayed messages in status DELAYED
         * @return created bucket entity
         * @throws DatabaseException
         */
        void UpdateQueueCounter(const std::string &queueArn, long messages, long size, long initial, long invisible, long delayed) const;

        /**
         * @brief Count the number of queues for a given region.
         *
         * @param prefix queue name prefix
         * @param region AWS region
         * @return number of queues in the given region.
         */
        [[nodiscard]] long CountQueues(const std::string &prefix = {}, const std::string &region = {}) const;

        /**
         * @brief Calculates the total size of all messages in the queue
         *
         * @param queueArn AWS queue ARN
         * @return total size of the queue
         */
        [[nodiscard]] long GetQueueSize(const std::string &queueArn) const;

        /**
         * @brief Deletes a queue.
         *
         * @param queue queue entity
         * @return number of deleted queues
         * @throws DatabaseException
         */
        long DeleteQueue(const Entity::SQS::Queue &queue) const;

        /**
         * @brief Deletes all queues
         *
         * @return number of deleted queues
         */
        long DeleteAllQueues() const;

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message CreateMessage(Entity::SQS::Message &message) const;

        /**
         * @brief Checks whether the message exists by the receipt handle.
         *
         * @param receiptHandle SQS message receipt handle
         * @return true, if the message exists, otherwise false
         * @throws Core::DatabaseException
         */
        [[nodiscard]] bool MessageExists(const std::string &receiptHandle) const;

        /**
         * @brief Checks whether the message exists by message ID
         *
         * @param messageId SQS message ID
         * @return true if the message exists, otherwise false
         * @throws Core::DatabaseException
         */
        [[nodiscard]] bool MessageExistsByMessageId(const std::string &messageId) const;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]] Entity::SQS::Message GetMessageById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]] Entity::SQS::Message GetMessageById(const std::string &oid) const;

        /**
         * @brief Returns a message by receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]] Entity::SQS::Message GetMessageByReceiptHandle(const std::string &receiptHandle) const;

        /**
         * @brief Returns a message by message ID
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]] Entity::SQS::Message GetMessageByMessageId(const std::string &messageId) const;

        /**
         * @brief Updates a given message.
         *
         * @param message SQS message
         * @return updated message
         */
        Entity::SQS::Message UpdateMessage(Entity::SQS::Message &message) const;

        /**
         * @brief Create a new queue or updates an existing message
         *
         * @param message message entity
         * @return created or updated SQS message entity
         * @throws DatabaseException
         */
        Entity::SQS::Message CreateOrUpdateMessage(Entity::SQS::Message &message) const;

        /**
         * @brief List all available resources
         *
         * @param region AWS region
         * @return list of SQS resources
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::MessageList ListMessages(const std::string &region = {}) const;

        /**
         * @brief Paged list of messages
         *
         * @param queueArn queue ARN
         * @param prefix message ID prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns array of sorting columns
         * @return list of SQS messages
         * @throws DatabaseException
         */
        [[nodiscard]] Entity::SQS::MessageList ListMessages(const std::string &queueArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const;

        /**
         * @brief Receive messages from an queue.
         *
         * @param queueArn queue ARN
         * @param visibility in seconds
         * @param maxResult maximal message count to receive
         * @param dlQueueArn dead letter queue ARN
         * @param maxRetries maximal number of retries
         * @param messageList message list
         */
        void ReceiveMessages(const std::string &queueArn, long visibility, long maxResult, const std::string &dlQueueArn, long maxRetries, Entity::SQS::MessageList &messageList) const;

        /**
         * @brief Reset expired resources
         *
         * @param queueArn ARN of the queue
         * @param visibility visibilityTimeout period in seconds
         * @return number of message resets
         */
        [[nodiscard]] long ResetMessages(const std::string &queueArn, long visibility) const;

        /**
         * @brief Redrive expired resources.
         *
         * @param queueArn ARN of the queue
         * @param redrivePolicy redrive policy
         * @return number of message sendto DLQ
         */
        [[nodiscard]] long RelocateToDlqMessages(const std::string &queueArn, const Entity::SQS::RedrivePolicy &redrivePolicy) const;

        /**
         * @brief Any message that has a DELAYED state is reset when the delay period is over.
         *
         * @param queueArn queue ARN.
         * @param delay delay in seconds.
         * @return number of updated queues
         */
        [[nodiscard]] long ResetDelayedMessages(const std::string &queueArn, long delay) const;

        /**
         * @brief Redrive message
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @param messageId message ID
         * @return total number of redriven messages
         */
        void RedriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId) const;

        /**
         * @brief Redrive messages
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @return total number of redriven messages
         */
        [[nodiscard]] long RedriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const;

        /**
         * @brief Any message, which has is older than the retention period is deleted.
         *
         * @param queueArn queue ARN.
         * @param retentionPeriod retention period in seconds.
         * @return number of messages deleted.
         */
        [[nodiscard]] long MessageRetention(const std::string &queueArn, long retentionPeriod) const;

        /**
         * @brief  Count the number of messages by state
         *
         * @param queueArn ARN of the queue
         * @param prefix message ID prefix
         * @return total number of messages
         */
        [[nodiscard]] long CountMessages(const std::string &queueArn = {}, const std::string &prefix = {}) const;

        /**
         * @brief Count the number of messages by state
         *
         * @param queueArn ARN of the queue
         * @return total message size
         */
        [[nodiscard]] long CountMessageSize(const std::string &queueArn = {}) const;

        /**
         * @brief Count the number of messages by state
         *
         * @param queueArn ARN of the queue
         * @param status message state
         */
        [[nodiscard]] long CountMessagesByStatus(const std::string &queueArn, const Entity::SQS::MessageStatus &status) const;

        /**
         * @brief Returns the average waiting time for messages in the given queue
         *
         * @par
         * Uses a simple min, max query to get the first and the last entry in the sqs_message collection. The average is then calculated as
         * (max-min)/2. This is done on a queue arn basis.
         *
         * @return map of average message waiting time per queue
         * @throws Core::DatabaseException
         */
        [[nodiscard]] Entity::SQS::MessageWaitTime GetAverageMessageWaitingTime() const;

        /**
         * @brief Import messages via bulk updates
7         *
         * @param queueArn queue ARN
         * @param messageArray
         */
        void ImportMessages(const std::string &queueArn, const value &messageArray) const;

        /**
         * @brief Deletes all messages of a queue
         *
         * @param queueArn message queue ARN to delete messages from
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]] auto DeleteMessages(const std::string &queueArn) const -> long;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]] auto DeleteMessage(const Entity::SQS::Message &message) const -> long;

        /**
         * @brief Deletes a message by its receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]] auto DeleteMessage(const std::string &receiptHandle) const -> long;

        /**
         * @brief Deletes a resources.
         *
         * @return total number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]] auto DeleteAllMessages() const -> long;

        /**
         * @brief Adjust all queue counters
         */
        void AdjustMessageCounters() const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Queue collection name
         */
        std::string _queueCollectionName;

        /**
         * Message collection name
         */
        std::string _messageCollectionName;

        /**
         * SQS in-memory database
         */
        SQSMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SQS_DATABASE_H
