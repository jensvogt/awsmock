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
         * Content type
         */
        std::string contentType;

        /**
         * Size
         */
        long size{};

        /**
         * Size
         */
        MessageStatus messageStatus = INITIAL;

        /**
         * Message attributes
         */
        std::vector<MessageAttributeCounter> messageAttributes;

        /**
         * Last sent time
         */
        system_clock::time_point lastSend;

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
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            r.message = Core::Json::GetStringValue(v, "message");
            r.contentType = Core::Json::GetStringValue(v, "contentType");
            r.size = Core::Json::GetLongValue(v, "size");
            r.messageStatus = MessageStatusFromString(v.at("messageStatus").as_string().data());
            r.messageAttributes = boost::json::value_to<std::vector<MessageAttributeCounter> >(v.at("messageAttributes"));
            r.lastSend = Core::DateTimeUtils::FromISO8601(v.at("lastSend").as_string().data());
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"requestId", obj.requestId},
                    {"user", obj.user},
                    {"contentType", obj.contentType},
                    {"topicArn", obj.topicArn},
                    {"messageId", obj.messageId},
                    {"message", obj.message},
                    {"size", obj.size},
                    {"messageStatus", MessageStatusToString(obj.messageStatus)},
                    {"messageAttributes", boost::json::value_from(obj.messageAttributes)},
                    {"lastSend", Core::DateTimeUtils::ToISO8601(obj.lastSend)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MESSAGE_COUNTER_H
