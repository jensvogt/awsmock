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
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            try {
                document rootDocument;
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "TopicArn", topicArn);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "MessageId", messageId);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "Message", message);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "Created", created);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "Modified", modified);

                if (!messageAttributes.empty()) {
                    array jsonMessageAttributeArray;
                    for (const auto &[fst, snd]: messageAttributes) {
                        document jsonAttribute;
                        jsonAttribute.append(kvp(fst, snd.ToDocument()));
                        jsonMessageAttributeArray.append(jsonAttribute);
                    }
                    rootDocument.append(kvp("MessageAttributes", jsonMessageAttributeArray));
                }

                return rootDocument.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Message tag_invoke(boost::json::value_to_tag<Message>, boost::json::value const &v) {
            Message r;
            r.topicArn = Core::Json::GetStringValue(v, "topicArn");
            r.messageId = Core::Json::GetStringValue(v, "messageId");
            r.message = Core::Json::GetStringValue(v, "message");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            r.messageAttributes = boost::json::value_to<std::map<std::string, MessageAttribute>>(v.at("messageAttributes"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Message const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicArn", obj.topicArn},
                    {"messageId", obj.messageId},
                    {"message", obj.message},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
                    {"messageAttributes", boost::json::value_from(obj.messageAttributes)},
            };
        }
    };

}// namespace AwsMock::Dto::SNS

#endif//AWSMOCK_DTO_SNS_MESSAGE_H
