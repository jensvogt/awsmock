//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SNS_EVENT_RECORD_H
#define AWSMOCK_DTO_SNS_EVENT_RECORD_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/sns/model/MessageAttribute.h>

namespace AwsMock::Dto::SNS {

    /**
     * @brief SNS Lambda notification event record
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Record final : Common::BaseCounter<Record> {

        /**
         * Message ID
         */
        std::string messageId;

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
         * MD5 sum
         */
        std::string md5Sum;

        /**
         * Event source
         */
        std::string eventSource;

        /**
         * Event source ARN
         */
        std::string eventSourceArn;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @param document DTO as BSON document
         */
        void FromDocument(const view_or_value<view, value> &document);

      private:

        friend Record tag_invoke(boost::json::value_to_tag<Record>, boost::json::value const &v) {
            Record r;
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            r.receiptHandle = Core::Json::GetStringValue(v, "ReceiptHandle");
            r.body = Core::Json::GetStringValue(v, "Body");
            r.md5Sum = Core::Json::GetStringValue(v, "MD5Sum");
            r.eventSource = Core::Json::GetStringValue(v, "EventSource");
            r.eventSourceArn = Core::Json::GetStringValue(v, "EventSourceArn");
            if (Core::Json::AttributeExists(v, "Attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("Attributes"));
            }
            if (Core::Json::AttributeExists(v, "MessageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("MessageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Record const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"messageId", obj.messageId},
                    {"receiptHandle", obj.receiptHandle},
                    {"body", obj.body},
                    {"md5Sum", obj.md5Sum},
                    {"eventSource", obj.eventSource},
                    {"eventSourceArn", obj.eventSourceArn},
                    {"attributes", boost::json::value_from(obj.attributes)},
                    {"messageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };
}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_EVENT_RECORD_H
