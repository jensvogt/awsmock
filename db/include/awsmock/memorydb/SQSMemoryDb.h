//
// Created by vogje01 on 11/19/23.
//

#ifndef AWSMOCK_REPOSITORY_SQS_MEMORYDB_H
#define AWSMOCK_REPOSITORY_SQS_MEMORYDB_H

// C++ includes
#include <string>

// Boost includes
#include <boost/container/map.hpp>
#include <boost/container/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/Linq.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringCollector.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/MessageWaitTime.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    using std::chrono::system_clock;

    /**
     * @brief SQS in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSMemoryDb {

    public:
        /**
         * @brief Constructor
         */
        explicit SQSMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static SQSMemoryDb &instance() {
            static SQSMemoryDb sqsMemoryDb;
            return sqsMemoryDb;
        }

        /**
         * @brief Check existence of queue
         *
         * @param region AWS region
         * @param name queue name
         * @return true if queue already exists
         * @throws DatabaseException
         */
        bool QueueExists(const std::string &region, const std::string &name);

        /**
         * @brief Check existence of queue
         *
         * @param region AWS region
         * @param queueUrl AWS region
         * @return true if queue already exists
         * @throws DatabaseException
         */
        bool QueueUrlExists(const std::string &region, const std::string &queueUrl);

        /**
         * @brief Check existence of queue by ARN
         *
         * @param queueArn queue ARN
         * @return true in case queue exists
         * @throws DatabaseException
         */
        bool QueueArnExists(const std::string &queueArn);

        /**
         * @brief Create a new queue in the SQS queue table
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue CreateQueue(const Entity::SQS::Queue &queue);

        /**
         * @brief Returns a queue by primary key
         *
         * @param oid queue primary key
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue GetQueueById(const std::string &oid);

        /**
         * @brief List all available queues
         *
         * @param region AWS region
         * @return List of SQS queues
         * @throws DatabaseException
         */
        Entity::SQS::QueueList ListQueues(const std::string &region = {});

        /**
         * @brief List available queues, using paging
         *
         * @param sortColumns vector of sort columns
         * @return List of SQS queues
         * @throws DatabaseException
         */
        Entity::SQS::QueueList ExportQueues(const std::vector<SortColumn> &sortColumns);

        /**
         * @brief Import s a single queue
         *
         * @par
         * During import pof a queue, the queue URL it adjusted, as it contains the hostname of the machine where the
         * queue is imported. Additionally, the counters are set to zero and the modified timestamp is adjusted.
         *
         * @param queue queue entity
         * @throws DatabaseException
         */
        void ImportQueue(Entity::SQS::Queue &queue);

        /**
         * @brief Returns a queue by ARN
         *
         * @param queueArn queue ARN
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue GetQueueByArn(const std::string &queueArn);

        /**
         * @brief Returns a queue by name and region
         *
         * @param region AWS region
         * @param queueName queue name
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue GetQueueByName(const std::string &region, const std::string &queueName);

        /**
         * @brief Returns a queue by URL
         *
         * @param queueUrl queue URL
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue GetQueueByUrl(const std::string &queueUrl);

        /**
         * @brief Returns a queue by DQL ARN
         *
         * @param dlqQueueArn queue ARN of the DQL
         * @return queue entity
         * @throws DatabaseException
         */
        Entity::SQS::Queue GetQueueByDlq(const std::string &dlqQueueArn);

        /**
         * @brief Purge a given queueUrl.
         *
         * @param queueArn queue ARN
         * @return total number of deleted messages
         */
        long PurgeQueue(const std::string &queueArn);

        /**
         * @brief Updates a given queue.
         *
         * @param queue AWS region
         * @return updated queue
         */
        Entity::SQS::Queue UpdateQueue(Entity::SQS::Queue &queue);

        /**
         * @brief Increases the number of invisible messages
         *
         * @param queueArn queue ARN
         * @param messageNumber number of invisible messages
         */
        void UpdateQueueInvisibleNumber(const std::string &queueArn, long messageNumber);

        /**
         * @brief Updates the counters of a queue
         *
         * @param queueArn queue ARN
         * @param keys number of keys
         * @param size bucket size
         * @param initial messages in status INITIAL
         * @param invisible messages in status INVISIBLE
         * @param delayed messages in status DELAYED
         * @return created bucket entity
         * @throws DatabaseException
         */
        void UpdateQueueCounter(const std::string &queueArn, long keys, long size, long initial, long invisible, long delayed);

        /**
         * @brief Count the number of queues for a given region.
         *
         * @param prefix queue name prefix
         * @param region AWS region
         * @return number of queues in the given region.
         */
        long CountQueues(const std::string &prefix = {}, const std::string &region = {}) const;

        /**
         * @brief Calculates the total size of all messages in the queue
         *
         * @param queueArn AWS queue ARN
         * @return total size of the queue
         */
        long GetQueueSize(const std::string &queueArn) const;

        /**
         * @brief Deletes a queue.
         *
         * @param queue queue entity
         * @return number of deleted queues
         * @throws DatabaseException
         */
        long DeleteQueue(const Entity::SQS::Queue &queue);

        /**
         * @brief Deletes all queues
         *
         * @return number of deleted queues
         * @throws DatabaseException
         */
        long DeleteAllQueues();

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message CreateMessage(const Entity::SQS::Message &message);

        /**
         * @brief Checks whether the message exists by receipt handle.
         *
         * @param receiptHandle receipt handle
         * @return true if message exists, otherwise false
         * @throws Core::DatabaseException
         */
        bool MessageExists(const std::string &receiptHandle) const;

        /**
         * @brief Checks whether the message exists by message ID
         *
         * @param messageId SQS message ID
         * @return true if message exists, otherwise false
         * @throws Core::DatabaseException
         */
        bool MessageExistsByMessageId(const std::string &messageId) const;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message GetMessageById(const std::string &oid);

        /**
         * @brief Updates a given message.
         *
         * @param message SQS message
         * @return updated message
         */
        Entity::SQS::Message UpdateMessage(Entity::SQS::Message &message);

        /**
         * @brief List all available resources
         *
         * @param region AWS region
         * @return list of SQS resources
         * @throws DatabaseException
         */
        Entity::SQS::MessageList ListMessages(const std::string &region = {});

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
        void ReceiveMessages(const std::string &queueArn, long visibility, long maxResult, const std::string &dlQueueArn, long maxRetries, Entity::SQS::MessageList &messageList);

        /**
         * @brief Reset expired resources
         *
         * @param queueArn ARN of the queue
         * @param visibility visibilityTimeout period in seconds
         * @return number of message resets
         */
        long ResetMessages(const std::string &queueArn, long visibility);

        /**
         * @brief Redrive expired resources.
         *
         * @param queueArn ARN of the queue
         * @param redrivePolicy redrive policy
         * @return number of message sendto DLQ
         */
        long RelocateToDlqMessages(const std::string &queueArn, const Entity::SQS::RedrivePolicy &redrivePolicy);

        /**
         * @brief Any message, which has a message state is DELAYED is reset when the delay period is over.
         *
         * @param queueArn queue URL.
         * @return number of delayed messages
         */
        long ResetDelayedMessages(const std::string &queueArn);

        /**
         * @brief Redrive a single message
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @param messageId message ID
         */
        void RedriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId);

        /**
         * @brief Redrive messages
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @return total number of redriven messages
         */
        long RedriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue);

        /**
         * @brief Any message, which has is older than the retention period is deleted.
         *
         * @param queueArn queue URL.
         * @param retentionPeriod retention period in seconds.
         * @return number of deleted messages
         */
        long MessageRetention(const std::string &queueArn, long retentionPeriod);

        /**
          * @brief Returns a message by receipt handle.
          *
          * @param receiptHandle message receipt handle
          * @return message entity
          * @throws Core::DatabaseException
          */
        Entity::SQS::Message GetMessageByReceiptHandle(const std::string &receiptHandle);

        /**
         * @brief Returns a message by message ID
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        Entity::SQS::Message GetMessageByMessageId(const std::string &messageId);

        /**
         * @brief Count the number of message by queue
         *
         * @param queueArn AWS queue ARN
         */
        long CountMessages(const std::string &queueArn = {});

        /**
         * @brief Count the number of message by state
         *
         * @param queueArn ARN of the queue
         * @param status message state
         */
        long CountMessagesByStatus(const std::string &queueArn, const Entity::SQS::MessageStatus &status);

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
        Entity::SQS::MessageWaitTime GetAverageMessageWaitingTime();

        /**
         * @brief Deletes all resources of a queue
         *
         * @param queue message queue to delete resources from
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        long DeleteMessages(const std::string &queue);

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        long DeleteMessage(const Entity::SQS::Message &message);

        /**
         * @brief Deletes a message by receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        long DeleteMessage(const std::string &receiptHandle);

        /**
         * @brief Deletes a resources.
         *
         * @return total number of message deleted
         * @throws Core::DatabaseException
         */
        long DeleteAllMessages();

    private:
        /**
         * SQS queue vector, when running without database
         */
        std::map<std::string, Entity::SQS::Queue> _queues{};

        /**
         * SQS message vector, when running without database
         */
        std::map<std::string, Entity::SQS::Message> _messages{};

        /**
         * Queue mutex
         */
        static boost::mutex _sqsQueueMutex;

        /**
         * Message mutex
         */
        static boost::mutex _sqsMessageMutex;
    };

} // namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SQS_MEMORYDB_H
