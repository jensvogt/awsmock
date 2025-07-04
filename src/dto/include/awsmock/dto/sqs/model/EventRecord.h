//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_EVENT_RECORD_H
#define AWSMOCK_DTO_SQS_EVENT_RECORD_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    /**
     * @brief SQS Lambda notification event record
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct EventRecord final : Common::BaseCounter<EventRecord> {

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
         * System attributes
         */
        std::map<std::string, std::string> attributes;

        /**
         * Message attributes
         */
        std::map<std::string, MessageAttribute> messageAttributes;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            try {
                document rootDocument;
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "awsRegion", region);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "messageId", messageId);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "receiptHandle", receiptHandle);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "body", body);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5OfBody", md5Sum);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "eventSource", eventSource);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "eventSourceARN", eventSourceArn);

                if (!messageAttributes.empty()) {
                    document jsonMessageAttributeObject;
                    for (const auto &[fst, snd]: messageAttributes) {
                        jsonMessageAttributeObject.append(kvp(fst, snd.ToDocument()));
                    }
                    rootDocument.append(kvp("messageAttributes", jsonMessageAttributeObject));
                }

                if (!attributes.empty()) {
                    document jsonAttributeObject;
                    jsonAttributeObject.append(kvp("ApproximateReceiveCount", "1"));
                    jsonAttributeObject.append(kvp("ApproximateFirstReceiveTimestamp", bsoncxx::types::b_int64(Core::DateTimeUtils::UnixTimestampMs(system_clock::now()))));
                    jsonAttributeObject.append(kvp("SenderId", Core::AwsUtils::CreateSQSSenderId()));
                    jsonAttributeObject.append(kvp("SentTimestamp", bsoncxx::types::b_int64(Core::DateTimeUtils::UnixTimestampMs(system_clock::now()))));
                    rootDocument.append(kvp("attributes", jsonAttributeObject));
                }
                return rootDocument.extract();
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @param document DTO as BSON document
         */
        void FromDocument(const view_or_value<view, value> &document) {
            try {
                region = Core::Bson::BsonUtils::GetStringValue(document, "awsRegion");
                messageId = Core::Bson::BsonUtils::GetStringValue(document, "messageId");
                receiptHandle = Core::Bson::BsonUtils::GetStringValue(document, "receiptHandle");
                body = Core::Bson::BsonUtils::GetStringValue(document, "body");
                md5Sum = Core::Bson::BsonUtils::GetStringValue(document, "md5OfBody");
                eventSource = Core::Bson::BsonUtils::GetStringValue(document, "eventSource");
                eventSourceArn = Core::Bson::BsonUtils::GetStringValue(document, "eventSourceArn");
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend EventRecord tag_invoke(boost::json::value_to_tag<EventRecord>, boost::json::value const &v) {
            EventRecord r;
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            r.receiptHandle = Core::Json::GetStringValue(v, "receiptHandle");
            r.body = Core::Json::GetStringValue(v, "body");
            r.md5Sum = Core::Json::GetStringValue(v, "md5Sum");
            r.eventSource = Core::Json::GetStringValue(v, "eventSource");
            r.eventSourceArn = Core::Json::GetStringValue(v, "eventSourceArn");
            if (Core::Json::AttributeExists(v, "attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("attributes"));
            }
            if (Core::Json::AttributeExists(v, "messageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("messageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EventRecord const &obj) {
            jv = {
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
}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_EVENT_RECORD_H
