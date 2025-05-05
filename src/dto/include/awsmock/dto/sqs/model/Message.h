//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_H
#define AWSMOCK_DTO_SQS_MESSAGE_H

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    /**
     * @brief SQS message entry as used by the SendMessageBatch
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Message final : Common::BaseCounter<Message> {

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * ID
         */
        std::string id;

        /**
         * Receipt handle
         */
        std::string receiptHandle;

        /**
         * Body
         */
        std::string body;

        /**
         * System attributes
         */
        std::map<std::string, std::string> attributes;

        /**
         * Message attributes
         */
        MessageAttributeList messageAttributes;

        /**
         * MD5 sum of the message body
         */
        std::string md5OfBody;

        /**
         * MD5 sum of message attributes
         */
        std::string md5OfMessageAttributes;

        /**
         * Created time stamp
         */
        system_clock::time_point created = Core::DateTimeUtils::LocalDateTimeNow();

        /**
         * Created time stamp
         */
        system_clock::time_point modified;

      private:

        friend Message tag_invoke(boost::json::value_to_tag<Message>, boost::json::value const &v) {
            Message r;
            r.id = v.at("id").as_string();
            r.messageId = v.at("messageId").as_string();
            r.receiptHandle = v.at("receiptHandle").as_string();
            r.body = v.at("body").as_string();
            r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("attributes"));
            r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("messageAttributes"));
            r.md5OfBody = v.at("md5OfBody").as_string();
            r.md5OfMessageAttributes = v.at("md5OfMessageAttributes").as_string();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Message const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MessageId", obj.messageId},
                    {"ReceiptHandle", obj.receiptHandle},
                    {"Body", obj.body},
                    {"Attributes", boost::json::value_from(obj.attributes)},
                    {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
                    {"MD5OfBody", obj.md5OfBody},
                    {"MD5OfMessageAttributes", obj.md5OfMessageAttributes},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ENTRY_H
