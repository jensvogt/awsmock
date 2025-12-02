//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_QUEUE_COUNTER_H
#define AWSMOCK_DTO_SQS_QUEUE_COUNTER_H

// C++ standard includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    struct QueueCounter final : Common::BaseCounter<QueueCounter> {

        /**
         * Queue URL
         */
        std::string queueUrl;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Queue name
         */
        std::string queueName;

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
         * Visibility timeout
         */
        long visibilityTimeout{};

        /**
         * Message delay
         */
        long delay{};

        /**
         * Retention period
         */
        long retentionPeriod{};

        /**
         * Maximal message size
         */
        long maxMessageSize{};

        /**
         * Total size of all messages in the queue
         */
        long size{};

        /**
         * Is DQL
         */
        bool isDlq{};

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Modified
         */
        system_clock::time_point modified;

      private:

        friend QueueCounter tag_invoke(boost::json::value_to_tag<QueueCounter>, boost::json::value const &v) {
            QueueCounter r;
            r.region = v.at("region").as_string();
            r.user = v.at("user").as_string();
            r.requestId = v.at("requestId").as_string();
            r.queueUrl = v.at("queueUrl").as_string();
            r.queueArn = v.at("queueArn").as_string();
            r.queueName = v.at("queueName").as_string();
            r.available = v.at("available").as_int64();
            r.invisible = v.at("invisible").as_int64();
            r.delayed = v.at("delayed").as_int64();
            r.visibilityTimeout = v.at("visibilityTimeout").as_int64();
            r.delay = v.at("delay").as_int64();
            r.retentionPeriod = v.at("retentionPeriod").as_int64();
            r.maxMessageSize = v.at("maxMessageSize").as_int64();
            r.size = (v.at("size").as_int64());
            r.isDlq = v.at("isDlq").as_bool();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, QueueCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueUrl", obj.queueUrl},
                    {"queueArn", obj.queueArn},
                    {"queueName", obj.queueName},
                    {"available", obj.available},
                    {"invisible", obj.invisible},
                    {"delayed", obj.delayed},
                    {"visibilityTimeout", obj.visibilityTimeout},
                    {"delay", obj.delay},
                    {"retentionPeriod", obj.retentionPeriod},
                    {"maxMessageSize", obj.maxMessageSize},
                    {"size", obj.size},
                    {"isDlq", obj.isDlq},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

    typedef std::vector<QueueCounter> QueueCounterList;

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_QUEUEATTRIBUTE_H
