//
// Created by vogje01 on 11/19/23.
//

#ifndef AWSMOCK_REPOSITORY_SQS_MEMORYDB_H
#define AWSMOCK_REPOSITORY_SQS_MEMORYDB_H

// C++ includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Macros.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/MessageWaitTime.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/repository/Database.h>

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
        AWSMOCK_API SQSMemoryDb() = default;

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
        AWSMOCK_API bool QueueExists(const std::string &region, const std::string &name);

        /**
         * @brief Check existence of queue
         *
         * @param region AWS region
         * @param queueUrl AWS region
         * @return true if queue already exists
         * @throws DatabaseException
         */
        AWSMOCK_API bool QueueUrlExists(const std::string &region, const std::string &queueUrl);

        /**
         * @brief Check existence of queue by ARN
         *
         * @param queueArn queue ARN
         * @return true in case queue exists
         * @throws DatabaseException
         */
        AWSMOCK_API bool QueueArnExists(const std::string &queueArn);

        /**
         * @brief Create a new queue in the SQS queue table
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::Queue CreateQueue(const Entity::SQS::Queue &queue);

        /**
         * @brief Returns a queue by primary key
         *
         * @param oid queue primary key
         * @return queue entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::Queue GetQueueById(const std::string &oid);

        /**
         * @brief List all available queues
         *
         * @param region AWS region
         * @return List of SQS queues
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::QueueList ListQueues(const std::string &region = {}) const;

        /**
         * @brief List available queues, using paging
         *
         * @param sortColumns vector of sort columns
         * @return List of SQS queues
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::QueueList ExportQueues(const std::vector<Core::SortColumn> &sortColumns);

        /**
         * @brief Returns a queue by ARN
         *
         * @param queueArn queue ARN
         * @return queue entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::Queue GetQueueByArn(const std::string &queueArn);

        /**
         * @brief Returns a queue by name and region
         *
         * @param region AWS region
         * @param queueName queue name
         * @return queue entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::Queue GetQueueByName(const std::string &region, const std::string &queueName);

        /**
         * @brief Returns a queue by URL
         *
         * @param queueUrl queue URL
         * @return queue entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::Queue GetQueueByUrl(const std::string &queueUrl);

        /**
         * @brief Returns a queue by DQL ARN
         *
         * @param dlqQueueArn queue ARN of the DQL
         * @return queue entity
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::Queue GetQueueByDlq(const std::string &dlqQueueArn);

        /**
         * @brief Purge a given queueUrl.
         *
         * @param queueArn queue ARN
         * @return total number of deleted messages
         */
        AWSMOCK_API long PurgeQueue(const std::string &queueArn);

        /**
         * @brief Updates a given queue.
         *
         * @param queue AWS region
         * @return updated queue
         */
        AWSMOCK_API Entity::SQS::Queue UpdateQueue(Entity::SQS::Queue &queue);

        /**
         * @brief Count the number of queues for a given region.
         *
         * @param prefix queue name prefix
         * @param region AWS region
         * @return number of queues in the given region.
         */
        AWSMOCK_API long CountQueues(const std::string &prefix = {}, const std::string &region = {}) const;

        /**
         * @brief Calculates the total size of all messages in the queue
         *
         * @param queueArn AWS queue ARN
         * @return total size of the queue
         */
        AWSMOCK_API long GetQueueSize(const std::string &queueArn) const;

        /**
         * @brief Deletes a queue.
         *
         * @param queue queue entity
         * @return number of deleted queues
         * @throws DatabaseException
         */
        AWSMOCK_API long DeleteQueue(const Entity::SQS::Queue &queue);

        /**
         * @brief Deletes all queues
         *
         * @return number of deleted queues
         * @throws DatabaseException
         */
        AWSMOCK_API long DeleteAllQueues();

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        AWSMOCK_API Entity::SQS::Message CreateMessage(const Entity::SQS::Message &message);

        /**
         * @brief Checks whether the message exists by receipt handle.
         *
         * @param receiptHandle receipt handle
         * @return true if message exists, otherwise false
         * @throws Core::DatabaseException
         */
        AWSMOCK_API bool MessageExists(const std::string &receiptHandle) const;

        /**
         * @brief Checks whether the message exists by message ID
         *
         * @param messageId SQS message ID
         * @return true if message exists, otherwise false
         * @throws Core::DatabaseException
         */
        AWSMOCK_API bool MessageExistsByMessageId(const std::string &messageId) const;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        AWSMOCK_API Entity::SQS::Message GetMessageById(const std::string &oid);

        /**
         * @brief Updates a given message.
         *
         * @param message SQS message
         * @return updated message
         */
        AWSMOCK_API Entity::SQS::Message UpdateMessage(Entity::SQS::Message &message);

        /**
         * @brief List all available resources
         *
         * @param region AWS region
         * @return list of SQS resources
         * @throws DatabaseException
         */
        AWSMOCK_API Entity::SQS::MessageList ListMessages(const std::string &region = {});

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
        AWSMOCK_API void ReceiveMessages(const std::string &queueArn, int visibility, int maxResult, const std::string &dlQueueArn, int maxRetries, Entity::SQS::MessageList &messageList);

        /**
         * @brief Reset expired resources
         *
         * @param queueArn ARN of the queue
         * @param visibility visibilityTimeout period in seconds
         * @return number of message resets
         */
        AWSMOCK_API long ResetMessages(const std::string &queueArn, long visibility);

        /**
         * @brief Redrive expired resources.
         *
         * @param queueArn ARN of the queue
         * @param redrivePolicy redrive policy
         * @return number of message sendto DLQ
         */
        AWSMOCK_API long RelocateToDlqMessages(const std::string &queueArn, const Entity::SQS::RedrivePolicy &redrivePolicy);

        /**
         * @brief Any message, which has a message state is DELAYED is reset when the delay period is over.
         *
         * @param queueArn queue URL.
         * @return number of delayed messages
         */
        AWSMOCK_API long ResetDelayedMessages(const std::string &queueArn);

        /**
         * @brief Redrive message
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @return total number of redriven messages
         */
        AWSMOCK_API long RedriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue);

        /**
         * @brief Any message, which has is older than the retention period is deleted.
         *
         * @param queueArn queue URL.
         * @param retentionPeriod retention period in seconds.
         * @return number of deleted messages
         */
        AWSMOCK_API long MessageRetention(const std::string &queueArn, long retentionPeriod);

        /**
          * @brief Returns a message by receipt handle.
          *
          * @param receiptHandle message receipt handle
          * @return message entity
          * @throws Core::DatabaseException
          */
        AWSMOCK_API Entity::SQS::Message GetMessageByReceiptHandle(const std::string &receiptHandle);

        /**
         * @brief Returns a message by message ID
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        AWSMOCK_API Entity::SQS::Message GetMessageByMessageId(const std::string &messageId);

        /**
         * @brief Count the number of message by queue
         *
         * @param queueArn AWS queue ARN
         */
        AWSMOCK_API long CountMessages(const std::string &queueArn = {});

        /**
         * @brief Count the number of message by state
         *
         * @param queueArn ARN of the queue
         * @param status message state
         */
        AWSMOCK_API long CountMessagesByStatus(const std::string &queueArn, Entity::SQS::MessageStatus status);

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
        AWSMOCK_API Entity::SQS::MessageWaitTime GetAverageMessageWaitingTime();

        /**
         * @brief Deletes all resources of a queue
         *
         * @param queue message queue to delete resources from
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        AWSMOCK_API long DeleteMessages(const std::string &queue);

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        AWSMOCK_API long DeleteMessage(const Entity::SQS::Message &message);

        /**
         * @brief Deletes a message by receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        AWSMOCK_API long DeleteMessage(const std::string &receiptHandle);

        /**
         * @brief Deletes a resources.
         *
         * @return total number of message deleted
         * @throws Core::DatabaseException
         */
        AWSMOCK_API long DeleteAllMessages();

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

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_SQS_MEMORYDB_H
