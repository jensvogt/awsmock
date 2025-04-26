//
// Created by vogje01 on 10/9/24.
//

#ifndef AWSMOCK_DTO_SNS_MESSAGE_COUNTER_H
#define AWSMOCK_DTO_SNS_MESSAGE_COUNTER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sns/model/MessageAttribute.h>
#include <awsmock/dto/sns/model/MessageAttributeCounter.h>
#include <awsmock/dto/sns/model/MessageStatus.h>

namespace AwsMock::Dto::SNS {

    struct MessageCounter final : Common::BaseCounter<MessageCounter> {

        /**
         * Topic ARM
         */
        std::string topicArn;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Message
         */
        std::string message;

        /**
         * Size
         */
        long size{};

        /**
         * Size
         */
        MessageStatus messageSatus;

        /**
         * Message attributes
         */
        std::vector<MessageAttributeCounter> messageAttributes;

        /**
         * Creation time
         */
        system_clock::time_point created;

        /**
         * Modification time
         */
        system_clock::time_point modified;

        /**
         * @brief Converts the DTO to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

      private:

        friend MessageCounter tag_invoke(boost::json::value_to_tag<MessageCounter>, boost::json::value const &v) {
            MessageCounter r;
            r.topicArn = v.at("topicArn").as_string();
            r.messageId = v.at("messageId").as_string();
            r.message = v.at("message").as_string();
            r.size = v.at("size").as_int64();
            r.messageSatus = MessageStatusFromString(v.at("messageStatus").as_string().data());
            r.messageAttributes = boost::json::value_to<std::vector<MessageAttributeCounter>>(v.at("messageAttributes"));
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageCounter const &obj) {
            jv = {
                    {"topicArn", obj.topicArn},
                    {"messageId", obj.messageId},
                    {"message", obj.message},
                    {"size", obj.size},
                    {"messageStatus", MessageStatusToString(obj.messageSatus)},
                    {"messageAttributes", boost::json::value_from(obj.messageAttributes)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MESSAGE_COUNTER_H
