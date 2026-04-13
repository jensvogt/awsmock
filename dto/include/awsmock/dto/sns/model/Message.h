//
// Created by vogje01 on 10/9/24.
//

#ifndef AWSMOCK_DTO_SNS_MESSAGE_H
#define AWSMOCK_DTO_SNS_MESSAGE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sns/model/MessageAttribute.h>

namespace AwsMock::Dto::SNS {

    struct Message final : Common::BaseCounter<Message> {

        /**
         * Topic ARM
         */
        std::string topicArn;

        /**
         * Target ARN
         */
        std::string targetArn;

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
         * Message attributes
         */
        MessageAttributeList messageAttributes;

        /**
         * Message size
         */
        long size{};

        /**
         * Creation time
         */
        system_clock::time_point created;

        /**
         * Modification time
         */
        system_clock::time_point modified;

      private:

        friend Message tag_invoke(boost::json::value_to_tag<Message>, boost::json::value const &v) {
            Message r;
            r.topicArn = Core::Json::GetStringValue(v, "TopicArn");
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            r.message = Core::Json::GetStringValue(v, "Message");
            r.contentType = Core::Json::GetStringValue(v, "ContentType");
            r.size = Core::Json::GetLongValue(v, "Created");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("Created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("Modified").as_string().data());
            r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("MessageAttributes"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Message const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"TopicArn", obj.topicArn},
                    {"MessageId", obj.messageId},
                    {"Message", obj.message},
                    {"ContentType", obj.contentType},
                    {"Size", obj.size},
                    {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif//AWSMOCK_DTO_SNS_MESSAGE_H
