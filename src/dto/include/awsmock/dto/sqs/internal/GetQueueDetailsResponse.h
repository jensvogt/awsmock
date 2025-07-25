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
#include <awsmock/core/logging/LogStream.h>
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
            r.queueName = Core::Json::GetStringValue(v, "queueName");
            r.queueUrl = Core::Json::GetStringValue(v, "queueUrl");
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            r.owner = Core::Json::GetStringValue(v, "owner");
            r.dlqArn = Core::Json::GetStringValue(v, "dlqArn");
            r.dlqMaxReceive = Core::Json::GetLongValue(v, "dlqMaxReceive");
            r.messageCount = Core::Json::GetLongValue(v, "messageCount");
            r.retentionPeriod = Core::Json::GetLongValue(v, "retentionPeriod");
            r.maxMessageSize = Core::Json::GetLongValue(v, "maxMessageSize");
            r.visibilityTimeout = Core::Json::GetLongValue(v, "visibilityTimeout");
            r.size = Core::Json::GetLongValue(v, "size");
            r.delay = Core::Json::GetLongValue(v, "delay");
            r.invisible = Core::Json::GetLongValue(v, "invisible");
            r.delayed = Core::Json::GetLongValue(v, "delayed");
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
