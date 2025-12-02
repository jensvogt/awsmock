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
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace AwsMock::Dto::SQS {

    using std::chrono::system_clock;

    /**
     * @brief SQS message entry as used by the SendMessageBatch
     *
     * @code
     * {
     *    "Id" : "0",
     *    "MessageBody" : "{\n  \"datenlieferantId\" : \"DLI398\",\n  \"filePathOriginaldatei\" : \"DLI398_20250801_PreisRef_02_01082025194921118.xml\",\n  \"anlieferungsdatum\" : \"2025-08-01T19:49:21.118\",\n  \"vlbPreisreferenzXml\" : \"<product><action>INSERT</action><isbn>9783515121613</isbn><vlb_record_id>ee1a0b08b67a4c198f68a08d5cf6ca99</vlb_record_id><is_digital>FALSE</is_digital><productform>BC</productform><title>Sakralität und Macht</title><fixed_retailprice><fixedprice_de>TRUE</fixedprice_de><fixedprice_at>TRUE</fixedprice_at></fixed_retailprice><last_price_mod>20190412</last_price_mod><price><market>DE</market><pricetype>10</pricetype><no_of_pieces>1</no_of_pieces><pricetext>Ladenpreis</pricetext><is_calculated>FALSE</is_calculated><is_provisional>FALSE</is_provisional><amount>49.00</amount><currency>EUR</currency><tax_component><component_id_gtin>9783515121613</component_id_gtin><component_productform>BC</component_productform><component_title>Sakralität und Macht</component_title><percent>7.00</percent><type>1</type><share>49.00</share></tax_component></price><price><market>AT</market><pricetype>10</pricetype><no_of_pieces>1</no_of_pieces><pricetext>Ladenpreis</pricetext><is_calculated>FALSE</is_calculated><is_provisional>FALSE</is_provisional><amount>50.40</amount><currency>EUR</currency><tax_component><component_id_gtin>9783515121613</component_id_gtin><component_productform>BC</component_productform><component_title>Sakralität und Macht</component_title><percent>10.00</percent><type>1</type><share>50.40</share></tax_component></price></product>\",\n  \"standDatum\" : \"2025-08-01T19:49:21.118\"\n}",
     *    "MessageAttributes" : {
     *      "file_type" : {
     *         "StringValue" : "VLB_PREISREFERENZ",
     *         "DataType" : "String"
     *      },
     *      "file_origin" : {
     *        "StringValue" : "FILE_PICKUP",
     *        "DataType" : "String"
     *      }
     *   }
     * }
     * @endcode
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
        std::map<std::string, MessageAttribute> messageSystemAttributes;

        /**
         * Message attributes
         */
        MessageAttributeList messageAttributes;

        /**
         * MD5 sum message body
         */
        std::string md5Body;

        /**
         * MD5 sum message attributes
         */
        std::string md5MessageAttributes;

        /**
         * MD5 sum system attributes
         */
        std::string md5SystemAttributes;

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
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5OfBody", md5Body);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5MessageAttributes", md5MessageAttributes);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "md5SystemAttributes", md5SystemAttributes);
                Core::Bson::BsonUtils::SetIntValue(rootDocument, "retries", retries);
                Core::Bson::BsonUtils::SetLongValue(rootDocument, "size", size);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "created", created);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "modified", modified);

                // Message attributes
                if (!messageAttributes.empty()) {
                    array jsonMessageAttributeArray;
                    for (const auto &[fst, snd]: messageAttributes) {
                        document jsonAttribute;
                        jsonAttribute.append(kvp(fst, snd.ToDocument()));
                        jsonMessageAttributeArray.append(jsonAttribute);
                    }
                    rootDocument.append(kvp("messageAttributes", jsonMessageAttributeArray));
                }

                // System attributes
                if (!messageSystemAttributes.empty()) {
                    array jsonAttributeArray;
                    for (const auto &[fst, snd]: messageSystemAttributes) {
                        document jsonAttribute;
                        jsonAttribute.append(kvp(fst, snd.ToDocument()));
                        jsonAttributeArray.append(jsonAttribute);
                    }
                    rootDocument.append(kvp("messageSystemAttributes", jsonAttributeArray));
                }
                return rootDocument.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to a DTO.
         *
         * @param object JSON object.
         */
        void FromDocument(const view_or_value<view, value> &object) {

            try {
                region = Core::Bson::BsonUtils::GetStringValue(object, "region");
                id = Core::Bson::BsonUtils::GetStringValue(object, "id");
                body = Core::Bson::BsonUtils::GetStringValue(object, "body");
                messageId = Core::Bson::BsonUtils::GetStringValue(object, "messageId");
                md5Body = Core::Bson::BsonUtils::GetStringValue(object, "md5Body");
                md5MessageAttributes = Core::Bson::BsonUtils::GetStringValue(object, "md5MessageAttributes");
                md5SystemAttributes = Core::Bson::BsonUtils::GetStringValue(object, "md5SystemAttributes");
                receiptHandle = Core::Bson::BsonUtils::GetStringValue(object, "receiptHandle");
                size = Core::Bson::BsonUtils::GetLongValue(object, "size");
                retries = Core::Bson::BsonUtils::GetLongValue(object, "retries");
                created = Core::Bson::BsonUtils::GetDateValue(object, "created");
                modified = Core::Bson::BsonUtils::GetDateValue(object, "modified");

                // Attributes
                if (object.view().find("messageAttributes") != object.view().end()) {
                    for (const bsoncxx::array::view attributesView{object.view()["messageAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                        MessageAttribute attribute;
                        std::string key = bsoncxx::string::to_string(attributeElement.key());
                        attribute.FromDocument(attributeElement.get_document().value);
                        messageAttributes[key] = attribute;
                    }
                }

                // System attributes
                if (object.view().find("messageSystemAttributes") != object.view().end()) {
                    for (const bsoncxx::array::view attributesView{object.view()["messageSystemAttributes"].get_array().value}; const bsoncxx::array::element &attributeElement: attributesView) {
                        MessageAttribute attribute;
                        std::string key = bsoncxx::string::to_string(attributeElement.key());
                        attribute.FromDocument(attributeElement.get_document().value);
                        messageAttributes[key] = attribute;
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
            r.md5Body = Core::Json::GetStringValue(v, "MD5OfBody");
            r.md5MessageAttributes = Core::Json::GetStringValue(v, "MD5OfMessageAttributes");
            r.md5SystemAttributes = Core::Json::GetStringValue(v, "MD5OfSystemAttributes");
            r.retries = Core::Json::GetLongValue(v, "Retries");
            r.size = Core::Json::GetLongValue(v, "Size");
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "Created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "Modified"));
            if (Core::Json::AttributeExists(v, "MessageSystemAttributes")) {
                r.messageSystemAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("MessageSystemAttributes"));
            }
            if (Core::Json::AttributeExists(v, "MessageAttributes")) {
                r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("MessageAttributes"));
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
                    {"Md5OfBody", obj.md5Body},
                    {"Retries", obj.retries},
                    {"Size", obj.size},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"MessageSystemAttributes", boost::json::value_from(obj.messageSystemAttributes)},
                    {"MessageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_MESSAGE_ENTRY_H
