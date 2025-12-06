//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_SEND_MESSAGE_RESPONSE_H
#define AWSMOCK_DTO_SQS_SEND_MESSAGE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/core/JsonUtils.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * Send message response
     *
     * Example:
     * @code{.json}
     * {
     *   "MD5OfMessageAttributes": "string",
     *   "MD5OfMessageBody": "string",
     *   "MD5OfMessageSystemAttributes": "string",
     *   "MessageId": "string",
     *   "SequenceNumber": "string"
     * }
     * @endcode
     */
    struct SendMessageResponse final : Common::BaseCounter<SendMessageResponse> {

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * MD5 sum of body
         */
        std::string md5Body;

        /**
         * MD5 sum of message attributes
         */
        std::string md5MessageAttributes;

        /**
         * MD5 sum of message system attributes
         */
        std::string md5MessageSystemAttributes;

        /**
         * Message sequence number
         */
        std::string sequenceNumber = Core::StringUtils::CreateRandomUuid();

      private:

        friend SendMessageResponse tag_invoke(boost::json::value_to_tag<SendMessageResponse>, boost::json::value const &v) {
            SendMessageResponse r;
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            r.md5Body = Core::Json::GetStringValue(v, "MD5Body");
            r.md5MessageAttributes = Core::Json::GetStringValue(v, "MD5MessageAttributes");
            r.md5MessageSystemAttributes = Core::Json::GetStringValue(v, "MD5MessageSystemAttributes");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SendMessageResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MessageId", obj.messageId},
                    {"SequenceNumber", obj.sequenceNumber},
                    {"MD5OfMessageBody", obj.md5Body},
                    {"MD5OfMessageAttributes", obj.md5MessageAttributes},
                    {"MD5OfMessageSystemAttributes", obj.md5MessageSystemAttributes},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_SEND_MESSAGE_RESPONSE_H
