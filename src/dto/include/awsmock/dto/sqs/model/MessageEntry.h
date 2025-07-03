//
// Created by vogje01 on 7/6/24.
//

#ifndef AWSMOCK_DTO_SQS_MESSAGE_ENTRY_H
#define AWSMOCK_DTO_SQS_MESSAGE_ENTRY_H

// C++ includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    /**
     * @brief SQS message entry as used by the SendMessageBatch
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MessageEntry final : Common::BaseCounter<MessageEntry> {

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
         * MD5 sum
         */
        std::string md5Sum;

        /**
         * Message size
         */
        long size = 0;

        /**
         * Message retries
         */
        long retries = 0;

        /**
         * Created time stamp
         */
        system_clock::time_point created = Core::DateTimeUtils::LocalDateTimeNow();

        /**
         * Created time stamp
         */
        system_clock::time_point modified;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document rootDocument;
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "id", id);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "messageId", messageId);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "receiptHandle", receiptHandle);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "body", body);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5OfBody", md5Sum);
                Core::Bson::BsonUtils::SetIntValue(rootDocument, "retries", retries);
                Core::Bson::BsonUtils::SetLongValue(rootDocument, "size", size);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "created", created);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "modified", modified);

                // Message attributes
                if (!messageAttributes.empty()) {
                    array jsonMessageAttributeArray;
                    for (const auto &[fst, snd]: messageAttributes) {
                        document jsonAttribute;
                        // TODO: Fix me
                        //jsonAttribute.append(kvp(fst, snd.ToDocument()));
                        jsonMessageAttributeArray.append(jsonAttribute);
                    }
                    rootDocument.append(kvp("messageAttributes", jsonMessageAttributeArray));
                }

                // System attributes
                if (!attributes.empty()) {
                    array jsonAttributeArray;
                    for (const auto &[fst, snd]: attributes) {
                        document jsonAttribute;
                        jsonAttribute.append(kvp(fst, snd));
                        jsonAttributeArray.append(jsonAttribute);
                    }
                    rootDocument.append(kvp("attributes", jsonAttributeArray));
                }
                return rootDocument.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param object JSON object.
         */
        void FromDocument(const view_or_value<view, value> &object) {

            try {
                region = Core::Bson::BsonUtils::GetStringValue(object, "Region");
                id = Core::Bson::BsonUtils::GetStringValue(object, "Id");
                body = Core::Bson::BsonUtils::GetStringValue(object, "MessageBody");

                // Attributes
                if (object.view().find("MessageAttributes") != object.view().end()) {
                    for (const bsoncxx::array::view attributesView{object.view()["MessageAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                        MessageAttribute attribute;
                        std::string key = bsoncxx::string::to_string(attributeElement.key());
                        // TODO: Fix me
                        //attribute.FromDocument(attributeElement.get_document().value);
                        messageAttributes[key] = attribute;
                    }
                }

                // System attributes
                if (object.view().find("MessageSystemAttributes") != object.view().end()) {

                    for (const bsoncxx::array::view attributesView{object.view()["MessageSystemAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                        std::string key = bsoncxx::string::to_string(attributeElement.key());
                        const std::string value = bsoncxx::string::to_string(attributeElement.get_string().value);
                        attributes[key] = value;
                    }
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend MessageEntry tag_invoke(boost::json::value_to_tag<MessageEntry>, boost::json::value const &v) {
            MessageEntry r;
            r.messageId = Core::Json::GetStringValue(v, "MessageId");
            r.id = Core::Json::GetStringValue(v, "Id");
            r.body = Core::Json::GetStringValue(v, "MessageBody");
            r.receiptHandle = Core::Json::GetStringValue(v, "ReceiptHandle");
            r.md5Sum = Core::Json::GetStringValue(v, "Md5OfBody");
            r.retries = Core::Json::GetLongValue(v, "retries");
            r.size = Core::Json::GetLongValue(v, "size");
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "modified"));
            if (Core::Json::AttributeExists(v, "attributes")) {
                r.attributes = boost::json::value_to<std::map<std::string, std::string>>(v.at("attributes"));
            }
            if (Core::Json::AttributeExists(v, "messageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, Dto::SQS::MessageAttribute>>(v.at("messageAttributes"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MessageEntry const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"MessageId", obj.messageId},
                    {"Id", obj.id},
                    {"MessageBody", obj.body},
                    {"Md5OfBody", obj.md5Sum},
                    {"Retries", obj.retries},
                    {"Size", obj.size},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"Attributes", boost::json::value_from(obj.attributes)},
                    {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ENTRY_H
