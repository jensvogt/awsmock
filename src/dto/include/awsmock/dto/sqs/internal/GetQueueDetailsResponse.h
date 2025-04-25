//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H
#define AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    /**
     * @brief Get queue details response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetQueueDetailsResponse final : Common::BaseCounter<GetQueueDetailsResponse> {

        /**
         * Queue name
         */
        std::string queueName;

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Number of messages
         */
        long messageCount{};

        /**
         * Retention period
         */
        long retentionPeriod{};

        /**
         * Maximal message size
         */
        long maxMessageSize{};

        /**
         * Visibility timeout
         */
        long visibilityTimeout{};

        /**
         * Size of all messages
         */
        long size{};

        /**
         * Delay
         */
        long delay{};

        /**
         * Queue owner
         */
        std::string owner;

        /**
         * Number of messages available
         */
        long available{};

        /**
         * Number of messages invisible
         */
        long invisible{};

        /**
         * Number of messages delayed
         */
        long delayed{};

        /**
         * Dead letter queue ARN
         */
        std::string dlqArn;

        /**
         * Maximal number of retries
         */
        long dlqMaxReceive{};

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

      private:

        friend GetQueueDetailsResponse tag_invoke(boost::json::value_to_tag<GetQueueDetailsResponse>, boost::json::value const &v) {
            GetQueueDetailsResponse r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.queueName = v.at("queueName").as_string();
            r.queueUrl = v.at("queueUrl").as_string();
            r.queueArn = v.at("queueArn").as_string();
            r.owner = v.at("owner").as_string();
            r.dlqArn = v.at("dlqArn").as_string();
            r.dlqMaxReceive = v.at("dlqMaxReceive").as_int64();
            r.messageCount = v.at("messageCount").as_int64();
            r.retentionPeriod = v.at("retentionPeriod").as_int64();
            r.maxMessageSize = v.at("maxMessageSize").as_int64();
            r.visibilityTimeout = v.at("visibilityTimeout").as_int64();
            r.size = v.at("size").as_int64();
            r.delay = v.at("delay").as_int64();
            r.invisible = v.at("invisible").as_int64();
            r.delayed = v.at("delayed").as_int64();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetQueueDetailsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueName", obj.queueName},
                    {"queueUrl", obj.queueUrl},
                    {"queueArn", obj.queueArn},
                    {"owner", obj.owner},
                    {"dlqArn", obj.dlqArn},
                    {"dlqMaxReceive", obj.dlqMaxReceive},
                    {"messageCount", obj.messageCount},
                    {"retentionPeriod", obj.retentionPeriod},
                    {"maxMessageSize", obj.maxMessageSize},
                    {"visibilityTimeout", obj.visibilityTimeout},
                    {"size", obj.size},
                    {"delay", obj.delay},
                    {"invisible", obj.invisible},
                    {"delayed", obj.delayed},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_GET_QUEUE_DETAILS_RESPONSE_H
