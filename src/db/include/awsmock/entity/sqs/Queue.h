//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_QUEUE_H
#define AWSMOCK_DB_ENTITY_SQS_QUEUE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/sqs/MessageAttribute.h>
#include <awsmock/entity/sqs/QueueAttribute.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::SQS {

    using std::chrono::system_clock;

    /**
     * SQS queue entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Queue /*final : Common::BaseEntity<Queue>*/ {

        /**
         * Region
         */
        std::string region;

        /**
         * ID
         */
        std::string oid;

        /**
         * Queue name
         */
        std::string name;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Queue userAttributes
         */
        QueueAttribute attributes;

        /**
         * Queue tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Pagination token
         */
        std::string paginationToken;

        /**
         * Pagination score
         */
        double score = 0.0;

        /**
         * Queue size in bytes
         */
        long size = 0;

        /**
         * DLQ flag
         */
        bool isDlq = false;

        /**
         * Content type
         */
        std::string contentType;

        /**
         * Main queue for this DLQ
         */
        std::string mainQueue;

        /**
         * Default message attributes
         */
        std::map<std::string, MessageAttribute> defaultMessageAttributes;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Converts the entity to a MongoDB document
         *
         * @return entity as MongoDB document.
         */
        [[maybe_unused]] [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the MongoDB document to an entity
         *
         * @param mResult MongoDB document.
         */
        Queue FromDocument(const std::optional<view> &mResult);
    };

    typedef std::vector<Queue> QueueList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_QUEUE_H
