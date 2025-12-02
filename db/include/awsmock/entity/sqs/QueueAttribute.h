//
// Created by vogje01 on 12/10/23.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_QUEUE_ATTRIBUTE_H
#define AWSMOCK_DB_ENTITY_SQS_QUEUE_ATTRIBUTE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/sqs/RedrivePolicy.h>

namespace AwsMock::Database::Entity::SQS {

    /**
     * @brief SQS queue attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct QueueAttribute {

        /**
         * Delay seconds
         *
         * <p>The length of time, in seconds, for which the delivery of all resources in the queue is delayed. Valid values: An integer from 0 to 900 (15 minutes). Default: 0.<p>
         */
        long delaySeconds = 0;

        /**
         * Max message size (256kB).
         *
         * <p> The limit of how many bytes a message can contain before Amazon SQS rejects it. Valid values: An integer from 1,024 bytes (1 KiB) up to 262,144 bytes (256 KiB). Default: 262,144 (256 KiB).</p>
         */
        long maxMessageSize = 262144;

        /**
         * Max retention period (4d).
         *
         * <p>The length of time, in seconds, for which Amazon SQS retains a message. Valid values: An integer representing seconds, from 60 (1 minute) to 1,209,600 (14 days). Default: 345,600 (4 days). When you
         * change a queue's attributes, the change can take up to 60 seconds for most of the attributes to propagate throughout the Amazon SQS system. Changes made to the MessageRetentionPeriod attribute can take
         * up to 15 minutes and will impact existing resources in the queue potentially causing them to be expired and deleted if the MessageRetentionPeriod is reduced below the age of existing resources.</p>
         */
        long messageRetentionPeriod = 345600;

        /**
         * Receive message timeout.
         *
         * <p>The length of time, in seconds, for which a ReceiveMessage action waits for a message to arrive. Valid values: An integer from 0 to 20 (seconds). Default: 0.</p>
         */
        long receiveMessageWaitTime = 20;

        /**
         * Visibility timeout.
         *
         * <p>The visibility timeout for the queue, in seconds. Valid values: An integer from 0 to 43,200 (12 hours). Default: 30. For more information about the visibility timeout, see Visibility Timeout in the Amazon SQS Developer Guide.</p>
         */
        long visibilityTimeout = 30;

        /**
         * Policy
         *
         * <p>The queue's policy. A valid AWS policy. For more information about policy structure, see Overview of AWS IAM Policies in the AWS Identity and Access Management User Guide.</p>
         */
        std::string policy;

        /**
         * Redrive policy
         *
         * <p>The string that includes the parameters for the dead-letter queue functionality of the source queue as a JSON object. The parameters are as follows:
         * <ul>
         * <li>deadLetterTargetArn – The Amazon Resource Name (ARN) of the dead-letter queue to which Amazon SQS moves resources after the value of maxReceiveCount is exceeded.</li>
         * <li>The number of times a message is delivered to the source queue before being moved to the dead-letter queue. Default: 10. When the ReceiveCount for a message exceeds the maxReceiveCount for a queue, Amazon SQS moves the message to the dead-letter-queue.</li>
         * </ul>
         * </p>
         */
        RedrivePolicy redrivePolicy;

        /**
         * Redrive allow policy
         *
         * <p>The string that includes the parameters for the permissions for the dead-letter queue redrive permission and which source queues can specify dead-letter queues as a JSON object. The parameters are as follows:
         * <ul>
         * <li>redrivePermission – The permission type that defines which source queues can specify the current queue as the dead-letter queue. Valid values are:
         * <ul>
         * <li>allowAll – (Default) Any source queues in this AWS account in the same Region can specify this queue as the dead-letter queue.</li>
         * <li>denyAll – No source queues can specify this queue as the dead-letter queue.</li>
         * <li>byQueue – Only queues specified by the sourceQueueArns parameter can specify this queue as the dead-letter queue.</li>
         * </ul>
         * </li>
         * <li>sourceQueueArns – The Amazon Resource Names (ARN)s of the source queues that can specify this queue as the dead-letter queue and redrive resources. You can specify this parameter only when the redrivePermission parameter
         * is set to byQueue. You can specify up to 10 source queue ARNs. To allow more than 10 source queues to specify dead-letter queues, set the redrivePermission parameter to allowAll.</li>
         * </ul>
         * </p>
         */
        std::string redriveAllowPolicy;

        /**
         * Number of messages
         */
        long approximateNumberOfMessages = 0;

        /**
         * Delay counter
         */
        long approximateNumberOfMessagesDelayed = 0;

        /**
         * Not visible counter
         */
        long approximateNumberOfMessagesNotVisible = 0;

        /**
         * AWS ARN
         */
        std::string queueArn;

        /**
         * Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]] void FromDocument(const std::optional<view> &mResult);
    };

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_QUEUE_ATTRIBUTE_H
