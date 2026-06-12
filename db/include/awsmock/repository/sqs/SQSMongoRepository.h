//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <queue>
#include <string>

// Boost includes
#include <boost/container/map.hpp>
#include <boost/container/string.hpp>

// MongoDB includes
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/MessageWaitTime.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/repository/sqs/SQSMemoryRepository.h>
#include <awsmock/utils/ConnectionPool.h>
#include <awsmock/utils/MongoUtils.h>
#include <awsmock/utils/SortColumn.h>
#include <awsmock/utils/SqsUtils.h>

namespace Awsmock::Database {

    using std::chrono::system_clock;

    /**
     * @brief SQS MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class SQSMongoRepository : public ISQSRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit SQSMongoRepository() = default;

        /**
         * @brief Check the existence of a queue
         *
         * @param region AWS region
         * @param name queue name
         * @return true if the queue already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool queueExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Check the existence of a queue
         *
         * @param region AWS region
         * @param queueUrl AWS region
         * @return true if the queue already exists
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
         * @brief Returns a queue by primary key
         *
         * @param oid queue primary key
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue getQueueById(const bsoncxx::oid &oid) const override;

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
         * @brief Returns a queue by URL
         *
         * @param region AWS region
         * @param queueUrl queue URL
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue getQueueByUrl(const std::string &region, const std::string &queueUrl) const override;

        /**
         * @brief Returns a queue by ARN
         *
         * @param queueArn queue ARN
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue getQueueByArn(const std::string &queueArn) const override;

        /**
         * @brief Returns a queue by DQL ARN
         *
         * @param dlqQueueArn queue ARN of the DQL
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue getQueueByDlq(const std::string &dlqQueueArn) const override;

        /**
         * @brief Checks whether a queue ARN is a dead letter queue
         *
         * @param queueArn queue ARN
         * @return list of main queues
         */
        [[nodiscard]]
        std::vector<Entity::SQS::Queue> isDlq(const std::string &queueArn) const override;

        /**
         * @brief Returns a queue by name and region
         *
         * @param region AWS region
         * @param queueName queue name
         * @return queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue getQueueByName(const std::string &region, const std::string &queueName) const override;

        /**
         * @brief List all available queues using paging
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
        Entity::SQS::QueueList listQueues(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const override;

        /**
         * @brief List all available queues in a aregion
         *
         * @param region AWS region
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::QueueList listQueues(const std::string &region) const override;


        /**
         * @brief List all available queues
         *
         * @return List of SQS queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::QueueList listQueues() const override;

        /**
         * @brief Export the queues
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
         * queue is imported. Additionally, the counters are set to zero and the modified timestamp is adjusted.
         *
         * @param queue queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue importQueue(Entity::SQS::Queue &queue) const override;

        /**
         * @brief Purge a given queueUrl.
         *
         * @param queueArn queue ARN
         * @return total number of deleted messages
         */
        [[nodiscard]]
        long purgeQueue(const std::string &queueArn) const override;

        /**
         * @brief Updates a given queue.
         *
         * @param queue AWS region
         * @return updated queue
         */
        [[nodiscard]]
        Entity::SQS::Queue updateQueue(Entity::SQS::Queue &queue) const override;

        /**
         * @brief Create a new queue or updates an existing queue
         *
         * @param queue queue entity
         * @return created SQS queue entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Queue createOrUpdateQueue(Entity::SQS::Queue &queue) const override;

        /**
         * @brief Count the number of queues for a given region.
         *
         * @param region AWS region
         * @param prefix queue name prefix
         * @return number of queues in the given region.
         */
        [[nodiscard]]
        long countQueues(const std::string &region, const std::string &prefix) const override;

        /**
         * @brief Deletes a queue.
         *
         * @param queue queue entity
         * @return number of deleted queues
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteQueue(const Entity::SQS::Queue &queue) const override;

        /**
         * @brief Deletes all queues
         *
         * @return number of deleted queues
         */
        [[nodiscard]]
        long deleteAllQueues() const override;

        /**
         * @brief Creates a new message in the SQS message table
         *
         * @param message SQS message entity
         * @return saved message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Message createMessage(Entity::SQS::Message &message) const override;

        /**
         * @brief Checks whether the message exists by the receipt handle.
         *
         * @param receiptHandle SQS message receipt handle
         * @return true, if the message exists, otherwise false
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        bool messageExists(const std::string &receiptHandle) const override;

        /**
         * @brief Checks whether the message exists by message ID
         *
         * @param messageId SQS message ID
         * @return true if the message exists, otherwise false
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
        [[nodiscard]]
        Entity::SQS::Message getMessageById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a message by ID.
         *
         * @param oid message objectId
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Message getMessageById(const std::string &oid) const override;

        /**
         * @brief Returns a message by receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]] Entity::SQS::Message getMessageByReceiptHandle(const std::string &receiptHandle) const override;

        /**
         * @brief Returns a message by message ID
         *
         * @param messageId message ID
         * @return message entity
         * @throws Core::DatabaseException
         */
        [[nodiscard]] Entity::SQS::Message getMessageByMessageId(const std::string &messageId) const override;

        /**
         * @brief Updates a given message.
         *
         * @param message SQS message
         * @return updated message
         */
        [[nodiscard]]
        Entity::SQS::Message updateMessage(Entity::SQS::Message &message) const override;

        /**
         * @brief Create a new queue or updates an existing message
         *
         * @param message message entity
         * @return created or updated SQS message entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::Message createOrUpdateMessage(Entity::SQS::Message &message) const override;

        /**
         * @brief List all available resources
         *
         * @param region AWS region
         * @return list of SQS resources
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::SQS::MessageList listMessages(const std::string &region) const override;

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
        [[nodiscard]]
        long resetMessages(const std::string &queueArn, long visibility) const override;

        /**
         * @brief Any message that has a DELAYED state is reset when the delay period is over.
         *
         * @param queueArn queue ARN.
         * @param delay delay in seconds.
         * @return number of updated queues
         */
        [[nodiscard]]
        long resetDelayedMessages(const std::string &queueArn, long delay) const override;

        /**
         * @brief Redrive message
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @param messageId message ID
         * @return total number of redriven messages
         */
        long redriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId) const override;

        /**
         * @brief Redrive messages
         *
         * @param originalQueue original queue
         * @param dlqQueue DLQ queue
         * @return total number of redriven messages
         */
        [[nodiscard]]
        long redriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const override;

        /**
         * @brief Redrive all messages, which has is older than the retention period is deleted.
         *
         * @param queueArn queue ARN.
         * @param retentionPeriod retention period in seconds.
         * @return number of messages deleted.
         */
        [[nodiscard]]
        long messageRetention(const std::string &queueArn, long retentionPeriod) const override;

        /**
         * @brief  Count the number of messages by state
         *
         * @param queueArn ARN of the queue
         * @param prefix message ID prefix
         * @return total number of messages
         */
        [[nodiscard]] long countMessages(const std::string &queueArn, const std::string &prefix) const override;

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
        [[nodiscard]] Entity::SQS::MessageWaitTime getAverageMessageWaitingTime() const override;

        /**
         * @brief Import messages via bulk updates
         *
         * @param queueArn queue ARN
         * @param messageArray
         */
        void importMessages(const std::string &queueArn, const value &messageArray) const override;

        /**
         * @brief Deletes all messages of a queue
         *
         * @param queueArn message queue ARN to delete messages from
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteMessages(const std::string &queueArn) const override;

        /**
         * @brief Deletes a message.
         *
         * @param message message to delete
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteMessage(const Entity::SQS::Message &message) const override;

        /**
         * @brief Deletes a message by its receipt handle.
         *
         * @param receiptHandle message receipt handle
         * @return number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteMessage(const std::string &receiptHandle) const override;

        /**
         * @brief Deletes a resource.
         *
         * @return total number of messages deleted
         * @throws Core::DatabaseException
         */
        [[nodiscard]]
        long deleteAllMessages() const override;

        /**
         * @brief Adjust all queue counters
         */
        void adjustMessageCounters() const override;

      private:

        /**
         * @brief Channeled log stream
         */
        mutable logger_t _logger{boost::log::keywords::channel = "SQS"};

        /**
         * @brief Database name
         */
        static constexpr auto _databaseName = "awsmock";

        /**
         * @brief Queue collection name
         */
        static constexpr auto _queueCollectionName = "sqs_queue";

        /**
         * @brief Message collection name
         */
        static constexpr auto _messageCollectionName = "sqs_message";
    };

}// namespace Awsmock::Database
