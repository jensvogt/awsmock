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
#include <awsmock/core/LogStream.h>
#include <awsmock/entity/common/BaseEntity.h>
#include <awsmock/entity/sqs/QueueAttribute.h>
#include <awsmock/utils/MongoUtils.h>

namespace AwsMock::Database::Entity::SQS {

    using std::chrono::system_clock;

    /**
     * SQS queue entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Queue final : Common::BaseEntity<Queue> {

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
         * Main queue for this DLQ
         */
        std::string mainQueue;

        /**
         * Creation date
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modification date
         */
        system_clock::time_point modified;

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

      private:

        friend Queue tag_invoke(boost::json::value_to_tag<Queue>, boost::json::value const &v) {
            Queue r;
            r.oid = v.at("oid").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.queueUrl = v.at("queueUrl").as_string();
            r.queueArn = v.at("queueArn").as_string();
            r.name = v.at("name").as_string();
            r.owner = v.at("owner").as_string();
            r.attributes = boost::json::value_to<QueueAttribute>(v.at("attributes"));
            r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("tags"));
            r.paginationToken = v.at("paginationToken").as_string();
            r.score = v.at("score").as_double();
            r.size = v.at("size").as_int64();
            r.isDlq = v.at("isDlq").as_bool();
            r.mainQueue = v.at("mainQueue").as_string();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Queue const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueUrl", obj.queueUrl},
                    {"queueArn", obj.queueArn},
                    {"name", obj.name},
                    {"owner", obj.owner},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"tags", boost::json::value_from(obj.tags)},
                    {"paginationToken", obj.paginationToken},
                    {"score", obj.score},
                    {"size", obj.size},
                    {"isDlq", obj.isDlq},
                    {"mainQueue", obj.mainQueue},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

    typedef std::vector<Queue> QueueList;

}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_QUEUE_H
