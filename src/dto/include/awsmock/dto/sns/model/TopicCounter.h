//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_TOPIC_COUNTER_H
#define AWSMOCK_DTO_SNS_TOPIC_COUNTER_H

// C++ standard includes
#include <chrono>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SNS {

    using std::chrono::system_clock;

    struct TopicCounter final : Common::BaseCounter<TopicCounter> {

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * Topic name
         */
        std::string topicName;

        /**
         * Retention period
         */
        long retentionPeriod{};

        /**
         * Maximal message size
         */
        long maxMessageSize{};

        /**
         * Total number of available messages
         */
        long messages{};

        /**
         * Total number of sent available messages
         */
        long messagesSend{};

        /**
         * Total number of resent available messages
         */
        long messagesResend{};

        /**
         * Total size of all messages in the topic
         */
        long size = 0;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Modified
         */
        system_clock::time_point modified;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend TopicCounter tag_invoke(boost::json::value_to_tag<TopicCounter>, boost::json::value const &v) {
            TopicCounter r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.topicName = Core::Json::GetStringValue(v, "topicName");
            r.retentionPeriod = Core::Json::GetLongValue(v, "retentionPeriod");
            r.maxMessageSize = Core::Json::GetLongValue(v, "maxMessageSize");
            r.messages = Core::Json::GetLongValue(v, "messages");
            r.messagesSend = Core::Json::GetLongValue(v, "messagesSend");
            r.messagesResend = Core::Json::GetLongValue(v, "messagesResend");
            r.size = Core::Json::GetLongValue(v, "size");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TopicCounter const &obj) {
            jv = {
                    {"topicArn", obj.topicArn},
                    {"topicName", obj.topicName},
                    {"retentionPeriod", obj.retentionPeriod},
                    {"maxMessageSize", obj.maxMessageSize},
                    {"messages", obj.messages},
                    {"messagesSend", obj.messagesSend},
                    {"messagesResend", obj.messagesResend},
                    {"size", obj.size},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_TOPIC_COUNTER_H
