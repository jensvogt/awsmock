//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/MessageWaitTime.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for SQS repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * SQS-related data.
     */
    class ISQSRepository {

      public:

        /**
         * @brief Virtual destructor for the ISQSRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~ISQSRepository() = default;

        /**
         * @brief Inserts a new queue or updates an existing one in the repository.
         *
         * If a module with the same identifier already exists, its data will be updated
         * with the provided queue information. Otherwise, a new queue will be added
         * to the repository.
         *
         * @param region The name of the AWS region.
         * @param name The bane of the queue.
         */
        [[nodiscard]]
        virtual bool queueExists(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Removes the specified element or elements from the collection or data structure.
         *
         * @param region The name of the AWS region.
         * @param queueUrl The URL of the queue.
         * @return true if the queue already exists.
         */
        [[nodiscard]]
        virtual bool queueUrlExists(const std::string &region, const std::string &queueUrl) const = 0;

        /**
         * @brief Check the existence of queue by ARN
         *
         * @param queueArn The queue ARN.
         * @return true in case the queue exists.
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool queueArnExists(const std::string &queueArn) const = 0;

        /**
         * @brief Create a new queue in the SQS queue table.
         *
         * @param queue The queue entity.
         * @return created SQS queue entity.
         * @throws DatabaseException.
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue createQueue(Entity::SQS::Queue &queue) const = 0;

        /**
         * @brief Returns a queue by its primary key.
         *
         * @param oid The queue primary key.
         * @return The queue entity.
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue getQueueById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns a queue by primary key
         *
         * @param oid The queue primary key
         * @return The queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue getQueueById(const std::string &oid) const = 0;

        /**
         * @brief Returns a queue by ARN
         *
         * @param queueArn The queue ARN
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue getQueueByArn(const std::string &queueArn) const = 0;

        /**
         * @brief Returns a queue by DQL ARN
         *
         * @param dlqQueueArn The queue ARN of the DQL
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue getQueueByDlq(const std::string &dlqQueueArn) const = 0;

        /**
         * @brief Checks whether a queue ARN is a dead letter queue
         *
         * @param queueArn queue ARN
         * @return list of main queues
         */
        [[nodiscard]]
        virtual std::vector<Entity::SQS::Queue> isDlq(const std::string &queueArn) const = 0;

        /**
         * @brief Returns a queue by name and region
         *
         * @param region AWS region
         * @param queueName queue name
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue getQueueByName(const std::string &region, const std::string &queueName) const = 0;

        /**
         * @brief Returns a queue by URL
         *
         * @param region AWS region
         * @param queueUrl queue URL
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue getQueueByUrl(const std::string &region, const std::string &queueUrl) const = 0;

        /**
         * @brief List of the available queues using paging
         *
         * @param prefix queue name prefix
         * @param pageSize maximal number of results
         * @param pageIndex pge index
         * @param sortColumns vector of sort columns
         * @param region AWS region
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::QueueList listQueues(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const = 0;

        /**
         * @brief List all available queues or a region
         *
         * @param region AWS region
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::QueueList listQueues(const std::string &region) const = 0;

        /**
         * @brief List all available queues
         *
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::QueueList listQueues() const = 0;

        /**
         * @brief List all available queue URLs of a region
         *
         * @param region AWS region
         * @param prefix name prefix
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual std::vector<std::string> listQueueUrls(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief List all available queue URLs
         *
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual std::vector<std::string> listQueueUrls() const = 0;

        /**
         * @brief List available queues using paging
         *
         * @param sortColumns vector of sort columns
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::QueueList exportQueues(const std::vector<SortColumn> &sortColumns) const = 0;

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
        [[nodiscard]]
        virtual Entity::SQS::Queue importQueue(Entity::SQS::Queue &queue) const = 0;

        /**
         * @brief Purge a given queueUrl.
         *
         * @param queueArn queue ARN
         * @return total number of deleted messages
         */
        [[nodiscard]]
        virtual long purgeQueue(const std::string &queueArn) const = 0;

        /**
         * @brief Updates a given queue.
         *
         * @param queue The name of the queue
         * @return updated queue
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue updateQueue(Entity::SQS::Queue &queue) const = 0;

        /**
         * @brief Create a new queue or updates an existing queue
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Queue createOrUpdateQueue(Entity::SQS::Queue &queue) const = 0;

        /**
         * @brief Count the number of queues for a given region.
         *
         * @param region AWS region
         * @param prefix queue name prefix
         * @return number of queues in the given region.
         */
        [[nodiscard]]
        virtual long countQueues(const std::string &region, const std::string &prefix) const = 0;

        /**
         * @brief Deletes a queue.
         *
         * @param queue queue entity
         * @return number of deleted queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteQueue(const Entity::SQS::Queue &queue) const = 0;

        /**
         * @brief Deletes all queues
         *
         * @return number of deleted queues
         */
        [[nodiscard]]
        virtual long deleteAllQueues() const = 0;

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Message createMessage(Entity::SQS::Message &message) const = 0;

        /**
         * @brief Checks whether the message exists by the receipt handle.
         *
         * @param receiptHandle SQS message receipt handle
         * @return true, if the message exists, otherwise false
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual bool messageExists(const std::string &receiptHandle) const = 0;

        /**
         * @brief Checks whether the message exists by message ID
         *
         * @param messageId SQS message ID
         * @return true if the message exists, otherwise false
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual bool messageExistsByMessageId(const std::string &messageId) const = 0;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Message getMessageById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Message getMessageById(const std::string &oid) const = 0;

        /**
         * @brief Returns a message by receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Message getMessageByReceiptHandle(const std::string &receiptHandle) const = 0;

        /**
         * @brief Returns a message by message ID
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Message getMessageByMessageId(const std::string &messageId) const = 0;

        /**
         * @brief Updates a given message.
         *
         * @param message SQS message
         * @return updated message
         */
        [[nodiscard]]
        virtual Entity::SQS::Message updateMessage(Entity::SQS::Message &message) const = 0;

        /**
         * @brief Create a new queue or updates an existing message
         *
         * @param message message entity
         * @return created or updated SQS message entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::Message createOrUpdateMessage(Entity::SQS::Message &message) const = 0;

        /**
         * @brief List all available resources
         *
         * @param region AWS region
         * @return list of SQS resources
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::SQS::MessageList listMessages(const std::string &region) const = 0;

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
        [[nodiscard]]
        virtual Entity::SQS::MessageList listMessages(const std::string &queueArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

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
        virtual void receiveMessages(const std::string &queueArn, long visibility, long maxResult, const std::string &dlQueueArn, long maxRetries, Entity::SQS::MessageList &messageList) const = 0;

        /**
         * @brief Reset expired resources
         *
         * @param queueArn ARN of the queue
         * @param visibility visibilityTimeout period in seconds
         * @return number of message resets
         */
        [[nodiscard]]
        virtual long resetMessages(const std::string &queueArn, long visibility) const = 0;

        /**
         * @brief Any message that has a DELAYED state is reset when the delay period is over.
         *
         * @param queueArn queue ARN.
         * @param delay delay in seconds.
         * @return number of updated queues
         */
        [[nodiscard]]
        virtual long resetDelayedMessages(const std::string &queueArn, long delay) const = 0;

        /**
         * @brief Redrive message
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @param messageId message ID
         * @return total number of redriven messages
         */
        [[nodiscard]]
        virtual long redriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId) const = 0;

        /**
         * @brief Redrive messages
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @return total number of redriven messages
         */
        [[nodiscard]]
        virtual long redriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const = 0;

        /**
         * @brief Any messages that have is older than the retention period are deleted.
         *
         * @param queueArn queue ARN.
         * @param retentionPeriod retention period in seconds.
         * @return number of messages deleted.
         */
        [[nodiscard]]
        virtual long messageRetention(const std::string &queueArn, long retentionPeriod) const = 0;

        /**
         * @brief  Count the number of messages by state
         *
         * @param queueArn ARN of the queue
         * @param prefix message ID prefix
         * @return total number of messages
         */
        [[nodiscard]]
        virtual long countMessages(const std::string &queueArn, const std::string &prefix) const = 0;

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
        [[nodiscard]]
        virtual Entity::SQS::MessageWaitTime getAverageMessageWaitingTime() const = 0;

        /**
         * @brief Import messages via bulk updates
7         *
         * @param queueArn queue ARN
         * @param messageArray
         */
        virtual void importMessages(const std::string &queueArn, const value &messageArray) const = 0;

        /**
         * @brief Deletes all messages of a queue
         *
         * @param queueArn message queue ARN to delete messages from
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual long deleteMessages(const std::string &queueArn) const = 0;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual long deleteMessage(const Entity::SQS::Message &message) const = 0;

        /**
         * @brief Deletes a message by its receipt handle.
         *
         * @param receiptHandle message receipt handle.
         * @return number of messages deleted.
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual long deleteMessage(const std::string &receiptHandle) const = 0;

        /**
         * @brief Deletes all messages.
         *
         * @return total number of messages deleted.
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllMessages() const = 0;

        /**
         * @brief Adjust all queue counters
         */
        virtual void adjustMessageCounters() const = 0;
    };

}// namespace Awsmock::Database