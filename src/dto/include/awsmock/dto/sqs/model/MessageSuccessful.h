//
// Created by vogje01 on 8/23/24.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_ENTRY_SUCCESS_H
#define AWSMOCK_DTO_SQS_MESSAGE_ENTRY_SUCCESS_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief SQS message entry success
     *
     * @code{.json}
     * {
     *   "Id": "string",
     *   "MD5OfMessageAttributes": "string",
     *   "MD5OfMessageBody": "string",
     *   "MD5OfMessageSystemAttributes": "string",
     *   "MessageId": "string",
     *   "SequenceNumber": "string"
     * }
     * @endcode
     */
    struct MessageSuccessful final : Common::BaseCounter<MessageSuccessful> {

        /**
         * Message ID
         */
        std::string id;

        /**
         * Message ID
         */
        std::string messageId;

        /**
         * Sequence
         */
        std::string sequenceNumber;

        /**
         * MD5 sum of body
         */
        std::string md5Body;

        /**
         * MD5 sum of sqs of user attributes
         */
        std::string md5MessageAttributes;

        /**
         * MD5 sum of sqs system attributes
         */
        std::string md5SystemAttributes;

      private:

        friend MessageSuccessful tag_invoke(boost::json::value_to_tag<MessageSuccessful>, boost::json::value const &v) {
            MessageSuccessful r;
            r.id = Core::Json::GetStringValue(v, "Id");
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            r.sequenceNumber = Core::Json::GetStringValue(v, "SequenceNumber");
            r.md5Body = Core::Json::GetStringValue(v, "MD5OfMessageBody");
            r.md5MessageAttributes = Core::Json::GetStringValue(v, "MD5OfMessageAttributes");
            r.md5SystemAttributes = Core::Json::GetStringValue(v, "MD5OfMessageSystemAttributes");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageSuccessful const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MessageId", obj.messageId},
                    {"Id", obj.id},
                    {"MessageId", obj.messageId},
                    {"SequenceNumber", obj.sequenceNumber},
                    {"MD5OfMessageBody", obj.md5Body},
                    {"MD5OfMessageAttributes", obj.md5MessageAttributes},
                    {"MD5OfMessageSystemAttributes", obj.md5SystemAttributes},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ENTRY_SUCCESS_H
