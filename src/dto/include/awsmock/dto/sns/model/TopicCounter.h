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
#include <awsmock/core/LogStream.h>
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
        long retentionPeriod = 0;

        /**
         * Maximal message size
         */
        long maxMessageSize = 0;

        /**
         * Total number of available messages
         */
        long availableMessages = 0;

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
            r.topicArn = v.at("topicArn").as_string();
            r.topicName = v.at("topicName").as_string();
            r.retentionPeriod = v.at("retentionPeriod").as_int64();
            r.maxMessageSize = v.at("maxMessageSize").as_int64();
            r.availableMessages = v.at("availableMessages").as_int64();
            r.size = v.at("size").as_int64();
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
                    {"availableMessages", obj.availableMessages},
                    {"size", obj.size},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_TOPIC_COUNTER_H
