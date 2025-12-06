//
// Created by vogje01 on 12/10/23.
//

#ifndef AWSMOCK_DB_ENTITY_SNS_TOPIC_ATTRIBUTE_H
#define AWSMOCK_DB_ENTITY_SNS_TOPIC_ATTRIBUTE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/entity/sqs/RedrivePolicy.h>

namespace AwsMock::Database::Entity::SNS {

    using std::chrono::system_clock;

    /**
     * SNS topic attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct TopicAttribute {

        /**
         * @brief Delivery policy
         *
         * <p>
         * The policy that defines how Amazon SNS retries failed deliveries to HTTP/S endpoints.
         * </p>
         */
        std::string deliveryPolicy;

        /**
         * @brief Display name.
         *
         * <p>
         * The display name to use for a topic with SMS subscriptions.
         * </p>
         */
        std::string displayName;

        /**
         * @brief FIFO topic
         *
         * <p>
         * Set to true to create a FIFO topic.
         * </p>
         */
        bool fifoTopic = false;

        /**
         * @brief Policy
         *
         * <p>
         * The policy that defines who can access your topic. By default, only the topic owner can publish or subscribe to
         * the topic.
         * </p>
         */
        std::string policy;

        /**
         * @brief Signature version
         *
         * <p>
         * The signature version corresponds to the hashing algorithm used while creating the signature of the notifications,
         * subscription confirmations, or unsubscribe confirmation resources sent by Amazon SNS. By default, SignatureVersion
         * is set to 1.
         * <p>
         */
        std::string signatureVersion;

        /**
         * @brief Tracing config
         *
         * <p>
         * Tracing mode of an Amazon SNS topic. By default TracingConfig is set to PassThrough, and the topic passes through
         * the tracing header it receives from an Amazon SNS publisher to its subscriptions. If set to Active, Amazon SNS
         * will vend X-Ray segment data to topic owner account if the sampled flag in the tracing header is true. This is
         * only supported on standard topics.
         * <p>
         */
        std::string tracingConfig;

        /**
         * @brief KMS master key ID
         *
         * <p>
         * The ID of an AWS managed customer master key (CMK) for Amazon SNS or a custom CMK. For more information, see
         * Key Terms. For more examples, see KeyId in the AWS Key Management Service API Reference.
         * </p>
         */
        std::string kmsMasterKeyId;

        /**
         * @brief Archiving policy.
         *
         * <p>
         * Adds or updates an inline policy document to archive resources stored in the specified Amazon SNS topic.
         * </p>
         */
        std::string archivePolicy;

        /**
         * @brief Beginning archive time.
         *
         * <p>
         * The earliest starting point at which a message in the topic’s archive can be replayed from. This point in time
         * is based on the configured message retention period set by the topic’s message archiving policy.
         * </p>
         */
        system_clock::time_point beginningArchiveTime;

        /**
         * @brief Content based deduplication.
         *
         * <p>
         * Enables content-based deduplication for FIFO topics.
         * </p>
         */
        bool contentBasedDeduplication = false;

        /**
         * @brief Number of available messages
         */
        long availableMessages = 0;

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document view.
         */
        [[maybe_unused]] void FromDocument(const std::optional<view> &mResult);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @param os output stream
         * @param r queue attribute
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const TopicAttribute &r);
    };

}// namespace AwsMock::Database::Entity::SNS

#endif// AWSMOCK_DB_ENTITY_SNS_TOPIC_ATTRIBUTE_H
